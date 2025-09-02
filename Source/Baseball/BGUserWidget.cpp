// Fill out your copyright notice in the Description page of Project Settings.


#include "BGUserWidget.h"

#include "BGPlayerController.h"
#include "Components/EditableTextBox.h"

void UBGUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(EditableTextBox))
	{
		if (EditableTextBox->OnTextCommitted.IsAlreadyBound(this, &UBGUserWidget::OnChatInputTextCommitted) == false)
		{
			EditableTextBox->OnTextCommitted.AddDynamic(this, &UBGUserWidget::OnChatInputTextCommitted);
		}
	}
}

void UBGUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (IsValid(EditableTextBox))
	{
		if (EditableTextBox->OnTextCommitted.IsAlreadyBound(this, &UBGUserWidget::OnChatInputTextCommitted) == true)
		{
			EditableTextBox->OnTextCommitted.RemoveDynamic(this, &UBGUserWidget::OnChatInputTextCommitted);
		}
	}
}

void UBGUserWidget::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (IsValid(GetOwningPlayer()))
		{
			ABGPlayerController* PC =  Cast<ABGPlayerController>(GetOwningPlayer());
			if (IsValid(PC))
			{
				PC->SetChatMessageString(Text.ToString());
				EditableTextBox->SetText(FText());
			}
		}
	}
}
