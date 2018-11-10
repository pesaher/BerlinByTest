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
	UFUNCTION(BlueprintCallable)
		bool Shoot();
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int32 GetCurrentAmmo() const;
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool HasAmmo() const;
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool HasMaximumAmmo() const;
	UFUNCTION(BlueprintCallable)
		void Reload(int32 InAmountOfAmmoToReload = 1);
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetRemainingReloadCooldownInSeconds() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void StartReload();
	FTimerManager& GetTimerManager(bool& bOutIsTimerManagerValid) const;

//VARIABLES
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration")
		int32 InitialAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Shooter|Configuration")
		int32 MaximumAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Shooter|Configuration")
		float ReloadCooldownInSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Configuration")
		TSubclassOf<AActor> ProjectileClass;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Shooter|Readables")
		int32 CurrentAmmo;
	UPROPERTY(BlueprintReadOnly, Category = "Projectile Shooter | Readables")
		FTimerHandle ReloadTimerHandle;
};
