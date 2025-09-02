// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BGGameModeBase.generated.h"

class ABGPlayerController;
/**
 * 
 */
UCLASS()
class BASEBALL_API ABGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;
	
	void PrintChatMessageString(ABGPlayerController* InChattingPlayerController, const FString& InChatMessageString);
	void IncreaseGuessCount(ABGPlayerController* InChattingPlayerController);
	void ResetGame();
	void JudgeGame(ABGPlayerController* InChattingPlayerController, int InStrikeCount);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ABGPlayerController>> AllPlayerControllers;

	TObjectPtr<ABGPlayerController> CurrentPlayerController;

	int32 CurrentTurnIndex = -1;
	FTimerHandle TurnTimerHandle;
	int32 CurrentTurnTime = 0;
	int32 MaxTurnTime = 10;

	void StartTurn();
	void StartTurnTimer();
	void HandleTurnTimeout();
	void TickTurnTimer();
};
