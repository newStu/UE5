// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthWidget.h"
#include "Components/ProgressBar.h"

void UHealthWidget::UpdateHealthPercent(float HealthPercent)
{
	HealthBar->SetPercent(HealthPercent);
}
