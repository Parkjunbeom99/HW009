// Fill out your copyright notice in the Description page of Project Settings.


#include "BGPlayerState.h"

#include "Net/UnrealNetwork.h"

ABGPlayerState::ABGPlayerState()
	: CurrentGuessCount(0)
	, MaxGuessCount(3)
	,PlayerNameString(TEXT("None"))
{
	bReplicates = true;
}

void ABGPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
	
}

FString ABGPlayerState::GetPlayerInfoString() const
{
	return PlayerNameString;
}
