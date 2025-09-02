// Fill out your copyright notice in the Description page of Project Settings.


#include "BGGameStateBase.h"

#include "BGPlayerController.h"
#include "Kismet/GameplayStatics.h"


void ABGGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABGPlayerController* BGPC = Cast<ABGPlayerController>(PC);
			if (IsValid(BGPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game");
				BGPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
