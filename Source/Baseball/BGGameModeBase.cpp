// Fill out your copyright notice in the Description page of Project Settings.


#include "BGGameModeBase.h"

#include "BGGameStateBase.h"
#include "BGPlayerController.h"
#include "BGPlayerState.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"


void ABGGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ResetGame();
	
}


void ABGGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	ABGPlayerController* CXPlayerController = Cast<ABGPlayerController>(NewPlayer);
	if (IsValid(CXPlayerController) == true)
	{
		CXPlayerController->ClientRPCPrintChatMessageString(FString("Connected to the game server."));
		AllPlayerControllers.Add(CXPlayerController);

		ABGPlayerState* BGPS = CXPlayerController->GetPlayerState<ABGPlayerState>();
		if (IsValid(BGPS) == true)
		{
			BGPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABGGameStateBase* CXGameStateBase =  GetGameState<ABGGameStateBase>();
		if (IsValid(CXGameStateBase) == true)
		{
			CXGameStateBase->MulticastRPCBroadcastLoginMessage(BGPS->PlayerNameString);
			
		}
	}
	if (AllPlayerControllers.Num() >= 2)
	{
		StartTurn();
	}

	
}

FString ABGGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABGGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

FString ABGGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);

}



void ABGGameModeBase::PrintChatMessageString(ABGPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{

	int32 Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);

	if (!IsGuessNumberString(GuessNumberString))
	{
	
		for (const auto& CXPlayerController : AllPlayerControllers)
		{
			if (IsValid(CXPlayerController))
			{
				CXPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
		return;
	}
	if (IsValid(CurrentPlayerController) == false || CurrentPlayerController  != InChattingPlayerController )
	{
		return;
	}


	GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
	ABGPlayerState* CXPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();

	IncreaseGuessCount(InChattingPlayerController);
	FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);


	FString CombinedMessageString = FString::Printf(TEXT("[%d/%d] %s -> %s"),
		CXPS->CurrentGuessCount, CXPS->MaxGuessCount, *InChatMessageString, *JudgeResultString);

	for (const auto& CXPlayerController : AllPlayerControllers)
	{
		if (IsValid(CXPlayerController))
		{
			CXPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
		}
	}
	
	int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
	JudgeGame(InChattingPlayerController, StrikeCount);
	if (StrikeCount != 3) 
	{
		StartTurn();
	}
}

void ABGGameModeBase::IncreaseGuessCount(ABGPlayerController* InChattingPlayerController)
{
	ABGPlayerState* CXPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
	if (IsValid(CXPS) == true)
	{
		CXPS->CurrentGuessCount++;
	}
}

void ABGGameModeBase::ResetGame()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	SecretNumberString = GenerateSecretNumber();
	for (const auto& CXPlayerController : AllPlayerControllers)
	{
		ABGPlayerState* CXPS = CXPlayerController->GetPlayerState<ABGPlayerState>();
		if (IsValid(CXPS) == true)
		{
			CXPS->CurrentGuessCount = 0;
		}
	}
	CurrentPlayerController = nullptr;
	CurrentTurnIndex = -1;
	StartTurn();

}

void ABGGameModeBase::JudgeGame(ABGPlayerController* InChattingPlayerController, int InStrikeCount)
{
	ABGPlayerState* CXPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
	
	if (InStrikeCount == 3)
	{
		FString WinMessage = CXPS->PlayerNameString + TEXT(" has won the game.");
		for (const auto& CXPlayerController : AllPlayerControllers)
		{
			if (IsValid(CXPlayerController))
				CXPlayerController->ClientRPCPrintChatMessageString(WinMessage);
		}
		ResetGame(); 
		return;
	}


	bool bIsDraw = true;
	for (const auto& BGPlayerController : AllPlayerControllers)
	{
		ABGPlayerState* PlayerState = BGPlayerController->GetPlayerState<ABGPlayerState>();
		if (IsValid(PlayerState) && PlayerState->CurrentGuessCount < PlayerState->MaxGuessCount)
		{
			bIsDraw = false;
			break;
		}
	}

	if (bIsDraw)
	{
		for (const auto& CXPlayerController : AllPlayerControllers)
		{
			if (IsValid(CXPlayerController))
				CXPlayerController->ClientRPCPrintChatMessageString(FString("Draw..."));
		}
		ResetGame(); 
	}
}

void ABGGameModeBase::StartTurn()
{
	int32 PlayerCount = AllPlayerControllers.Num();
	if (PlayerCount == 0) 
	{
		CurrentPlayerController = nullptr;
		return;
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(TurnTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
	}
	CurrentTurnIndex++;
	CurrentTurnIndex = CurrentTurnIndex % PlayerCount;
	CurrentPlayerController = AllPlayerControllers[CurrentTurnIndex];
	if (IsValid(CurrentPlayerController))
	{
		StartTurnTimer();
	}
}

void ABGGameModeBase::StartTurnTimer()
{
	CurrentTurnTime = MaxTurnTime;
	
	GetWorld()->GetTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ABGGameModeBase::TickTurnTimer,
		1.0f,
		true
	);
	
	for (auto& PC : AllPlayerControllers)
	{
		PC->ClientRPCUpdateTurnTime(CurrentTurnTime);
	}
		
	

}

void ABGGameModeBase::HandleTurnTimeout()
{
	GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
	
	if (IsValid(CurrentPlayerController))
	{
		IncreaseGuessCount(CurrentPlayerController);
		StartTurn();	
	}
}

void ABGGameModeBase::TickTurnTimer()
{
	CurrentTurnTime--;
	
	for (auto& PC : AllPlayerControllers)
	{
		if (IsValid(PC))
			PC->ClientRPCUpdateTurnTime(CurrentTurnTime);
	}

	if (CurrentTurnTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
		HandleTurnTimeout();
	}
}


