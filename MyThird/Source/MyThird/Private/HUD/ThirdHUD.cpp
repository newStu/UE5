// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ThirdHUD.h"
#include  "HUD/HealthWidget.h"

void AThirdHUD::CreateHealthWidget()
{
	 APlayerController* PlayerController = GetOwningPlayerController();
	if (HealthWidgetClass != nullptr)
	{
		HealthWidget = CreateWidget<UHealthWidget>(PlayerController, HealthWidgetClass);
		HealthWidget->AddToViewport();
	}
}

void AThirdHUD::UpdateHealthWidget(float HealthPrecent)
{
	if (HealthWidget != nullptr)
	{
		HealthWidget->UpdateHealthPercent(HealthPrecent);
	}
}
