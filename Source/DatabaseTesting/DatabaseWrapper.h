#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DatabaseWrapper.generated.h"


USTRUCT(BlueprintType)
struct FTietomaaAnswers {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 resultCount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FString> ResultColumns;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) TArray<FString> ResultRows;
};

UCLASS(BlueprintType)
class DATABASETESTING_API UDatabaseWrapper : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable) bool Connect(FString _DB_directory); // open database
	UFUNCTION(BlueprintCallable) void Disconnect(); // close database

	UFUNCTION(BlueprintCallable) bool CreateTable(FString _TableName, TArray<int32> Answers);

	UFUNCTION(BlueprintCallable) bool InsertData(FString _TableName, TArray<int32> Answers);
	UFUNCTION(BlueprintCallable) bool UpdateData(FString _TableName, TArray<int32> Answers, int32 column);

	UFUNCTION(BlueprintCallable) bool Query(FString sqlClause, FTietomaaAnswers& Asnwers);
	UFUNCTION(BlueprintCallable) bool QueryAllSimilarAnswers(FString _TableName, int32 col, TArray<int32> _answers, FTietomaaAnswers& Answers);

	UFUNCTION(BlueprintCallable) void SetTableName(FString _TableName); // open database
	//void BeginDestroy() override;

	UPROPERTY(BlueprintReadWrite) FString TableName = "DEFAULT_TABLE_NAME";
	struct sqlite3* DB = nullptr;
	int32 EntryCount = 0;

	int32 GetEntryCount();

	static int ResultsCallback(void* whatIsThis, int resultCount, char** resultRows, char** resultColumns);

};
