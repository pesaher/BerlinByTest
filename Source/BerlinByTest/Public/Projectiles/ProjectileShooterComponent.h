// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "ProjectileShooterComponent.generated.h"

UCLASS( ClassGroup=(Projectiles), meta=(BlueprintSpawnableComponent) )
class BERLINBYTEST_API UProjectileShooterComponent : public UActorComponent
{
	GENERATED_BODY()

//FUNCTIONS
public:	
	// Sets default values for this component's properties
	UProjectileShooterComponent();
	// Shoots a projectile
	UFUNCTION(BlueprintCallable)
		bool Shoot();
	// Return the current available amount of projectiles to shoot
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int32 GetCurrentAmmo() const;
	// Returns true if there is at least a projectile available to shoot, false otherwise
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool HasAmmo() const;
	// Returns true if the component cannot hold any more projectiles, false otherwise
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool HasMaximumAmmo() const;
	// Adds the selected amount of projectiles to the current ammo
	UFUNCTION(BlueprintCallable)
		void Reload(int32 InAmountOfAmmoToReload = 1);
	// Returns how many seconds are left until the next projectile is added automatically to the inventory
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetRemainingReloadCooldownInSeconds() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Sets a timer to reload a projectile
	UFUNCTION()
		void StartReload();
	// Returns the current world timer manager
	FTimerManager& GetTimerManager(bool& bOutIsTimerManagerValid) const;
	// Computes the actor which should be auto-aimed, if any
	const AActor* GetCenteredShootableActor() const;
	/** Generates a score which dictates the actor that should be auto-aimed depending on the angle,
		distance and priority of the actor */
	float GetAutoAimScore(float InPriority, float InDistance, float InCosineOfVisionAngle, float InCosineOfMaximumVisionAngle) const;

//VARIABLES
public:
	// How many projectiles should this actor start with
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration|Ammo")
		int32 InitialAmmo;
	// How many projectiles will this actor be able to hold at most at the same time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Shooter|Configuration|Ammo")
		int32 MaximumAmmo;
	// How many seconds will it take for a projectile to be automatically reloaded after being shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Shooter|Configuration|Ammo")
		float ReloadCooldownInSeconds;
	// The projectiles spawned will be of this class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration|Ammo")
		TSubclassOf<AActor> ProjectileClass;
	/** How much will the auto-aim priority of the actor weight on the auto-aim decision.
		E.g., if a priority weight of 1 is selected, distance weight of 0 and focus weight of 3,
		the priority will weight a 25% in the decision of who to auto-aim to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration|Auto Aim")
		float PriorityWeight;
	/** How much will the distance to the actor weight on the auto-aim decision.
		Distance weight will only be taken into account if the maximum distance variable is higher than 0.
		E.g., if a priority weight of 5 is selected, distance weight of 1 and focus weight of 4,
		the distance will weight a 10% in the decision of who to auto-aim to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration|Auto Aim")
		float DistanceWeight;
	/** Maximum distance at which an actor will be considered for auto-aiming.
		If lower or equal to 0, there will be no limit to the distance at which an actor will be considered,
		however distance will have no weight in the calculation of the most suitable objective */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration|Auto Aim")
		float MaximumDistance;
	/** How much will how much an actor is centered on the screen weight on the auto-aim decision.
		E.g., if a priority weight of 2 is selected, distance weight of 2 and focus weight of 2,
		how centered the actor is will weight a 33.33% in the decision of who to auto-aim to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration|Auto Aim")
		float FocusWeight;
	// The maximum angle (in degrees) from the center of the screen at which the projectiles can auto-aim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration|Auto Aim")
		float MaximumVisionAngle;

protected:
	// Number of projectiles held at the moment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Readables")
		int32 CurrentAmmo;
	// Timer handle used for the reload cooldown
	UPROPERTY(BlueprintReadOnly, Category = "Projectile Shooter|Readables")
		FTimerHandle ReloadTimerHandle;
};
