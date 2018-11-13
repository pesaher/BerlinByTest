// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shootables/Shootable.h"
#include "ProjectileObjective.generated.h"

class UStaticMeshComponent;

UCLASS()
class BERLINBYTEST_API AProjectileObjective : public AActor, public IShootable
{
	GENERATED_BODY()

//FUNCTIONS
public:
	// Sets default values for this actor's properties
	AProjectileObjective();
	virtual float GetAutoAimPriority_Implementation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

//VARIABLES
public:
	// This value must be between 0 and 10 (it will be clamped to that range in the calculations otherwise)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile Objective|Shootable")
		float AutoAimPriority;
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile Objective")
		UStaticMeshComponent* MeshComponent;
};
