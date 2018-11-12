// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectiles/Shootable.h"
#include "ProjectileObjective.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
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
	UFUNCTION()
		void BeginHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

//VARIABLES
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile Objective|Shootable")
		float AutoAimPriority;
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile Objective")
		USphereComponent* CollisionComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile Objective")
		UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovementComponent;
};
