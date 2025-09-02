// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BGNotifyWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class BASEBALL_API UBGNotifyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void PrintNofiicationString(const FString &InString);
	void ClearNotification();

	void PrintTimer(int32 Time);

	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI" , meta =  (BindWidget))
	TObjectPtr<UTextBlock> NotifyBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI" , meta =  (BindWidget))
	TObjectPtr<UTextBlock> CountingBlock;

	FTimerHandle RemoveTimerHandle;
	
	
	
};
