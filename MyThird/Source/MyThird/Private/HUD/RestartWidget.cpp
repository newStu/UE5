// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/RestartWidget.h"

#include "Components/Button.h"
#include "Game/ThirdPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void URestartWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (RestartButton != nullptr)
	{
		RestartButton->OnClicked.AddDynamic(this, &URestartWidget::OnRestartButtonClicked);
	}
	
	if (ExitButton != nullptr)
	{
		ExitButton->OnClicked.AddDynamic(this, &URestartWidget::OnExitButtonClicked);
	}
}

void URestartWidget::OnRestartButtonClicked()
{
	AThirdPlayerController* PlayerController =  Cast<AThirdPlayerController>(GetOwningPlayer());
	if (PlayerController != nullptr)
	{
		PlayerController->HideRestartWidget();
	}
	
	UGameplayStatics::OpenLevel(this, FName(UGameplayStatics::GetCurrentLevelName(this))); 
}

void URestartWidget::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}
