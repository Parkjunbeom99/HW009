// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BGUserWidget.generated.h"

class UEditableTextBox;
/**
 * 
 */
UCLASS()
class BASEBALL_API UBGUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);



protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI" , meta =  (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox;
};
