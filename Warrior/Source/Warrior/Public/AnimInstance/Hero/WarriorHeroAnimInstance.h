// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/WarriorCharacterAnimInstance.h"
#include "WarriorHeroAnimInstance.generated.h"


class AWarriorHeroCharacter;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	// Hero角色的引用，后期使用
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AnimData|Refrences")
	AWarriorHeroCharacter* OwningHeroCharacter;
	
	// 是否处于放松状态
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="AnimData|LocomtionData")
	bool bShouldEnterRelaxState;
	
	// 5秒进入放松状态
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AnimData|LocomtionData")
	float EnterRelaxStateThreshold = 5.f;
	
	// 空闲经过时间
	float IdleElpasedTime;
};