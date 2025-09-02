// Fill out your copyright notice in the Description page of Project Settings.


#include "BGNotifyWidget.h"

#include "Components/TextBlock.h"

void UBGNotifyWidget::PrintNofiicationString(const FString& InString)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RemoveTimerHandle);

		NotifyBlock->SetText(FText::FromString(InString));

		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &UBGNotifyWidget::ClearNotification);

		World->GetTimerManager().SetTimer(RemoveTimerHandle, TimerDel, 5.0f, false);
	}
}

void UBGNotifyWidget::ClearNotification()
{
	if (NotifyBlock)
	{
		NotifyBlock->SetText(FText());
	}
}

void UBGNotifyWidget::PrintTimer(int32 Time)
{
	if (CountingBlock)
	{
		CountingBlock->SetText(FText::AsNumber(Time));
	}
}
