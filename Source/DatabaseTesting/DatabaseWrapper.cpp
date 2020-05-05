#include "DatabaseWrapper.h"
#include "sqlite/sqlite3.h"


// CONNECT TO DATABASE
bool UDatabaseWrapper::Connect(FString DB_directory)
{
	if (sqlite3_open(TCHAR_TO_ANSI(*DB_directory), &DB) == SQLITE_OK) {
		UE_LOG(LogTemp, Warning, TEXT("Database opened or created if did not exists."));
		//SetDatabaseDirectory(DB_directory);
		return true;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Database could not be opened!"))
			sqlite3_close(DB);
		DB = nullptr;
		return false;
	}
}


// DISCONNECT DATABASE
void UDatabaseWrapper::Disconnect()
{
	if (DB) {
		sqlite3_close(DB);
	}
}


bool UDatabaseWrapper::CreateTable(FString _TableName, TArray<int32> Answers)
{
	int32 arrayCount = Answers.Num();
	char* msgErr;

	// construct sql clause
	FString sql =
		"CREATE TABLE IF NOT EXISTS " + _TableName + " ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT, ";

	for (int32 i = 0; i < arrayCount; i++) {
		sql.Append("V");
		sql.AppendInt(i + 1);

		if (i == arrayCount - 1) {
			sql.Append(" INTEGER NOT NULL);");
		}
		else {
			sql.Append(" INTEGER NOT NULL,");
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *sql);

	sqlite3_exec(DB, TCHAR_TO_UTF8(*sql), NULL, 0, &msgErr);

	return true;
}


// INSERT RECORD (answer array)
bool UDatabaseWrapper::InsertData(FString _TableName, TArray<int32> answers)
{
	int32 arrayCount = answers.Num();
	char* msgErr;

	// construct sql clause
	FString sql =
		"INSERT INTO ";
	sql.Append(_TableName);
	sql.Append("(");

	// Select columns
	for (int32 i = 0; i < arrayCount; i++) {
		sql.Append("V");
		sql.AppendInt(i + 1);
		if (i != arrayCount - 1) sql.Append(",");
	}
	sql.Append(") VALUES(");

	// Add values to columns
	for (int32 i = 0; i < arrayCount; i++) {
		sql.AppendInt(answers[i]);
		if (i != arrayCount - 1) sql.Append(",");
	}

	sql.Append(");");
	UE_LOG(LogTemp, Warning, TEXT("INSERT DATA: %s"), *sql);
	sqlite3_exec(DB, TCHAR_TO_ANSI(*sql), NULL, 0, &msgErr);

	return true;
}


bool UDatabaseWrapper::UpdateData(FString _TableName, TArray<int32> Answers, int32 column)
{

	FString sql = "UPDATE ";
	sql.Append(_TableName);
	sql.Append(" SET V");
	sql.AppendInt(column + 1);
	sql.Append(" = ");
	sql.AppendInt(Answers[column]);
	sql.Append(" WHERE ID = ");
	sql.AppendInt(GetEntryCount());
	sql.Append(";");

	sqlite3_exec(DB, TCHAR_TO_ANSI(*sql), NULL, 0, NULL);

	UE_LOG(LogTemp, Warning, TEXT("Update data: %s"), *sql);
	UE_LOG(LogTemp, Warning, TEXT("Entry Count: %d"), GetEntryCount());
	return true;
}


// QUERY DATA FROM TABLE
bool UDatabaseWrapper::Query(FString sqlClause, FTietomaaAnswers& Answers)
{

	struct FTietomaaAnswers stats;

	sqlite3_exec(DB, TCHAR_TO_ANSI(*sqlClause), ResultsCallback, &stats, NULL);

	Answers = stats;
	EntryCount = stats.resultCount;
	return false;
}


bool UDatabaseWrapper::QueryAllSimilarAnswers(FString _TableName, int32 col, TArray<int32> _answers, FTietomaaAnswers& Answers)
{
	// construct sql clause
	FString sql = "SELECT * FROM ";
	sql.Append(_TableName);
	sql.Append(" WHERE ");

	for (int32 i = 0; i < col + 1; ++i) {
		sql.Append("V");
		sql.AppendInt(i + 1);
		sql.Append(" = ");
		sql.AppendInt(_answers[i]);
		if (i != col) sql.Append(" AND ");
	}

	sql.Append(";");

	UE_LOG(LogTemp, Warning, TEXT("QUERY ALL: %s"), *sql);
	struct FTietomaaAnswers stats;

	sqlite3_exec(DB, TCHAR_TO_ANSI(*sql), ResultsCallback, &stats, NULL);

	Answers = stats;

	return false;
}


int32 UDatabaseWrapper::GetEntryCount()
{
	return EntryCount;
}

int UDatabaseWrapper::ResultsCallback(void* whatIsThis, int resultCount, char** resultRows, char** resultColumns)
{
	FTietomaaAnswers* stats = (FTietomaaAnswers*)whatIsThis;

	//UE_LOG(LogTemp, Warning, TEXT("Result count: %s"), stats->resultCount);

	for (int32 i = 0; i < resultCount; i++) {
		stats->ResultColumns.Add(resultColumns[i]); // <- Zero idea if this does anything.
		stats->ResultRows.Add(resultRows[i]);
	}
	stats->resultCount = stats->ResultColumns.Num() / (int32)resultCount;

	FString result = "";
	//UE_LOG(LogTemp, Warning, TEXT("Result count: %s"), *FString::FromInt(resultCount));

	for (int32 i = 0; i < resultCount; i++) {
		result.Append(resultColumns[i]);
		result.Append(": ");
		result.Append(resultRows[i]);
		result.Append(" ");

		//UE_LOG(LogTemp, Warning, TEXT("Hmm: %s"), *result);
	}

	return 0;
}


// SETTERS
//void UDatabaseWrapper::SetDatabaseDirectory(FString _DB_Filepath)
//{
//	DB_directory2 = _DB_Filepath;
//}

void UDatabaseWrapper::SetTableName(FString _tableName)
{
	TableName = _tableName;
}
