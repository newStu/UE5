// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

#include "Interface/HealthInterface.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UHealthComponent::LoseHealth(float Amount)
{
	// 已死亡则不再结算,防止 Death 重复触发(叠加 Widget)
	if (Health <= 0)
	{
		return;
	}

	Health -= Amount;
	if (GetOwner()->Implements<UHealthInterface>())
	{
		IHealthInterface::Execute_Damage(GetOwner());
	}
	
	if (Health <= 0)
	{
		Health = 0.f;
		if (GetOwner()->Implements<UHealthInterface>())
		{
			IHealthInterface::Execute_Death(GetOwner());
		}
	}
}
