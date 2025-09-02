# 숫자 야구 게임 (Unreal Engine C++)

## 소개
이 프로젝트는 Unreal Engine C++을 사용하여 구현한 **멀티플레이어 숫자 야구 게임**입니다.  
플레이어가 번갈아가며 3자리 숫자를 맞추고, 스트라이크(S)와 볼(B)로 승패를 판정합니다.  
게임은 채팅 메시지를 통해 진행되며, 턴별로 진행됩니다.

---

## 주요 기능
- Secret Number 생성 (중복 없는 3자리 난수)
- 플레이어 입력 검증 (3자리, 중복 없음, 0 제외)
- 스트라이크/볼 판정
- 시도 횟수 관리 및 턴 진행
- 승리/무승부 판정 및 게임 리셋
- 채팅 기반 입력 처리 및 메시지 브로드캐스트

---

## 1. Secret Number 생성

```cpp
FString ABGGameModeBase::GenerateSecretNumber()
{
    TArray<int32> Numbers;
    for (int32 i = 1; i <= 9; ++i)
        Numbers.Add(i);

    FMath::RandInit(FDateTime::Now().GetTicks());

    FString Result;
    for (int32 i = 0; i < 3; ++i)
    {
        int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
        Result.Append(FString::FromInt(Numbers[Index]));
        Numbers.RemoveAt(Index);
    }

    return Result;
}

```
---

## 2. 플레이어 입력 검증

```cpp
bool ABGGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
    if (InNumberString.Len() != 3)
        return false;

    TSet<TCHAR> UniqueDigits;
    for (TCHAR C : InNumberString)
    {
        if (!FChar::IsDigit(C) || C == '0')
            return false;
        UniqueDigits.Add(C);
    }

    return UniqueDigits.Num() == 3;
}
```
---

## 3. 스트라이크/볼 판정

```cpp
FString ABGGameModeBase::JudgeResult(
    const FString& InSecretNumberString, 
    const FString& InGuessNumberString)
{
    int32 StrikeCount = 0, BallCount = 0;

    for (int32 i = 0; i < 3; ++i)
    {
        if (InSecretNumberString[i] == InGuessNumberString[i])
            StrikeCount++;
        else if (InSecretNumberString.Contains(FString::Printf(TEXT("%c"), InGuessNumberString[i])))
            BallCount++;
    }

    if (StrikeCount == 0 && BallCount == 0)
        return TEXT("OUT");

    return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}
```

## 4. 게임 리셋 및 턴 관리

```cpp
void ABGGameModeBase::ResetGame()
{
    SecretNumberString = GenerateSecretNumber();
    CurrentPlayerController = nullptr;
    CurrentTurnIndex = -1;

    for (auto& CXPlayerController : AllPlayerControllers)
    {
        auto CXPS = CXPlayerController->GetPlayerState<ABGPlayerState>();
        if (IsValid(CXPS))
            CXPS->CurrentGuessCount = 0;
    }

    StartTurn();
}

void ABGGameModeBase::StartTurn()
{
    CurrentTurnIndex = (CurrentTurnIndex + 1) % AllPlayerControllers.Num();
    CurrentPlayerController = AllPlayerControllers[CurrentTurnIndex];
    StartTurnTimer();
}

```

## 5. 플레이어 시도 처리 및 승리/무승부 판정

```cpp
void ABGGameModeBase::PrintChatMessageString(ABGPlayerController* InPlayer, const FString& Message)
{
    FString Guess = Message.RightChop(Message.Len() - 3);

    if (!IsGuessNumberString(Guess) || CurrentPlayerController != InPlayer)
        return;

    IncreaseGuessCount(InPlayer);
    FString Result = JudgeResult(SecretNumberString, Guess);

    // 메시지 브로드캐스트
    for (auto& PC : AllPlayerControllers)
        PC->ClientRPCPrintChatMessageString(FString::Printf(TEXT("%s -> %s"), *Guess, *Result));

    // 승리/무승부 판정
    int32 Strikes = FCString::Atoi(*Result.Left(1));
    JudgeGame(InPlayer, Strikes);
}


```

## 6. 시도 횟수 증가
```cpp
void ABGGameModeBase::IncreaseGuessCount(ABGPlayerController* InPlayer)
{
    auto CXPS = InPlayer->GetPlayerState<ABGPlayerState>();
    if (IsValid(CXPS))
        CXPS->CurrentGuessCount++;
}

```

## 7. 게임 승리/무승부 판정
```cpp
void ABGGameModeBase::JudgeGame(ABGPlayerController* InPlayer, int32 Strikes)
{
    auto CXPS = InPlayer->GetPlayerState<ABGPlayerState>();
    if (Strikes == 3)
    {
        FString WinMessage = CXPS->PlayerNameString + TEXT(" has won the game.");
        for (auto& PC : AllPlayerControllers)
            PC->ClientRPCPrintChatMessageString(WinMessage);
        ResetGame();
        return;
    }

    bool bIsDraw = true;
    for (auto& PC : AllPlayerControllers)
    {
        auto PS = PC->GetPlayerState<ABGPlayerState>();
        if (IsValid(PS) && PS->CurrentGuessCount < PS->MaxGuessCount)
        {
            bIsDraw = false;
            break;
        }
    }

    if (bIsDraw)
    {
        for (auto& PC : AllPlayerControllers)
            PC->ClientRPCPrintChatMessageString(TEXT("Draw..."));
        ResetGame();
    }
}


```

## 8. 턴 타이머
```cpp
void ABGGameModeBase::StartTurnTimer()
{
    CurrentTurnTime = MaxTurnTime;
    GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, this, &ABGGameModeBase::TickTurnTimer, 1.0f, true);

    for (auto& PC : AllPlayerControllers)
        PC->ClientRPCUpdateTurnTime(CurrentTurnTime);
}

void ABGGameModeBase::TickTurnTimer()
{
    CurrentTurnTime--;

    for (auto& PC : AllPlayerControllers)
        PC->ClientRPCUpdateTurnTime(CurrentTurnTime);

    if (CurrentTurnTime <= 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
        HandleTurnTimeout();
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



```

