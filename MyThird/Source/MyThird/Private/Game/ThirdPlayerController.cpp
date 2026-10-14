// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ThirdPlayerController.h"
#include "HUD/RestartWidget.h"
#include "HUD/ThirdHUD.h"


void AThirdPlayerController::ShowRestartWidget()
{
	if (ReStartWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReStartWidgetClass 未配置,请在 PlayerController 蓝图的中设置 Widget Class"));
		return;
	}

	if (RestartWidget != nullptr)
	{
		// 已有 Widget 时不再重复创建,避免叠加
		return;
	}

	SetPause(true);
	SetInputMode(FInputModeUIOnly());

	bShowMouseCursor = true;
	RestartWidget = CreateWidget<URestartWidget>(this, ReStartWidgetClass);
	RestartWidget->AddToViewport();
}

void AThirdPlayerController::HideRestartWidget()
{
	if (RestartWidget != nullptr)
	{
		RestartWidget->RemoveFromParent();
		RestartWidget = nullptr;
	}

	SetPause(false);
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AThirdPlayerController::UpdateHealthPercent(float HealthPercent)
{
	// if (HealthWidget != nullptr)
	// {
	// 	HealthWidget->UpdateHealthPercent(HealthPercent);
	// }
	
	if (ThirdHUD != nullptr)
	{
		ThirdHUD->UpdateHealthWidget(HealthPercent);
	}
}

void AThirdPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// if (HealthWidgetClass !=nullptr)
	// {
	// 	HealthWidget = CreateWidget<UHealthWidget>(this, HealthWidgetClass);
	// 	HealthWidget->AddToViewport();
	// }
	
	ThirdHUD = Cast<AThirdHUD>(GetHUD());
	if (ThirdHUD != nullptr)
	{
		ThirdHUD->CreateHealthWidget();
	}
}
