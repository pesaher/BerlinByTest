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
	// This function will be called whenever a projectile hits this actor
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Projectiles")
		void ProjectileHit();
	/** This function is used to get the auto-aim priority of this actor.
		It must return a value between 0 and 10, and has to be implemented in every actor that implements this interface */
	UFUNCTION(BlueprintNativeEvent, Category = "Projectiles")
		float GetAutoAimPriority() const;
	virtual float GetAutoAimPriority_Implementation() const;
};
