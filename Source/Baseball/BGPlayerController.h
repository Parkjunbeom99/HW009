// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BGPlayerController.generated.h"

class UBGNotifyWidget;
class UBGUserWidget;
/**
 * 
 */
UCLASS()
class BASEBALL_API ABGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABGPlayerController();
	
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintMessageString(const FString& InMessage);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCUpdateTurnTime(int32 Timer);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBGUserWidget> InputUIClass;

	UPROPERTY()
	TObjectPtr<UBGUserWidget> InputUIWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UBGNotifyWidget> NotifyUIClass;

	UPROPERTY()
	TObjectPtr<UBGNotifyWidget> NotifyUIWidget;


	
};
