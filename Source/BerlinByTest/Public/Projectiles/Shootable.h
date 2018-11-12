// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Shootable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShootable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BERLINBYTEST_API IShootable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectiles")
		void ProjectileHit();
	UFUNCTION(BlueprintNativeEvent, Category = "Projectiles")
		float GetAutoAimPriority() const;
	virtual float GetAutoAimPriority_Implementation() const;
};
