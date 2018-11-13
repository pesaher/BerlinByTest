// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileActor.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class BERLINBYTEST_API AProjectileActor : public AActor
{
	GENERATED_BODY()

//FUNCTIONS
public:
	// Sets default values for this actor's properties
	AProjectileActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// This function will be called whenever the projectile has a blocking hit with other actor
	UFUNCTION()
		void BeginHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

//VARIABLES
public:
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		USphereComponent* CollisionComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovementComponent;
};
