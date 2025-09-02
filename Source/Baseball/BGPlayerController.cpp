// Fill out your copyright notice in the Description page of Project Settings.


#include "BGPlayerController.h"

#include "BGGameModeBase.h"
#include "BGNotifyWidget.h"
#include "BGPlayerState.h"
#include "BGUserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


ABGPlayerController::ABGPlayerController()
{
	bReplicates = true;
}

void ABGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	if (IsLocalController() == true) // client owning
	{
		 // Show InputWIdget
		if (IsValid(InputUIClass)) 
		{
			InputUIWidget = CreateWidget<UBGUserWidget>(this, InputUIClass);
			if (IsValid(InputUIWidget))
			{
				InputUIWidget->AddToViewport();
			}
		}
		// Show InputWIdget
		if (IsValid(NotifyUIClass)) 
		{
			NotifyUIWidget = CreateWidget<UBGNotifyWidget>(this, NotifyUIClass);
			if (IsValid(NotifyUIWidget))
			{
				NotifyUIWidget->AddToViewport();
			}
			else
			{
				UKismetSystemLibrary::PrintString(this,"xxxxx");
			}
		}
	}
}

void ABGPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	if (IsLocalController() == true)
	{
		ABGPlayerState* PS = GetPlayerState<ABGPlayerState>();
		if (IsValid(PS) == true)
		{
			FString CombinedMessageString = PS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			ServerRPCPrintMessageString(CombinedMessageString);
		}
	}
}

void ABGPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	if (IsValid(NotifyUIWidget))
	{
		NotifyUIWidget->PrintNofiicationString(InChatMessageString);
	}
}

void ABGPlayerController::ClientRPCUpdateTurnTime_Implementation(int32 Timer)
{
	if (IsValid(NotifyUIWidget))
	{
		NotifyUIWidget->PrintTimer(Timer);
	}
}

void ABGPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);	
}

void ABGPlayerController::ServerRPCPrintMessageString_Implementation(const FString& InMessage)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ABGGameModeBase* BGGM = Cast<ABGGameModeBase>(GM);
		if (IsValid(BGGM) == true)
		{
			BGGM->PrintChatMessageString(this, InMessage);
		}
	}
}







