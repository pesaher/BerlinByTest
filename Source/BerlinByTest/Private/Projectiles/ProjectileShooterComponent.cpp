// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Projectiles/ProjectileShooterComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values for this component's properties
UProjectileShooterComponent::UProjectileShooterComponent()
{
	MaximumAmmo = 6;
	InitialAmmo = MaximumAmmo;
	ReloadCooldownInSeconds = 5.f;
}

// Called when the game starts
void UProjectileShooterComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmo = InitialAmmo;
	StartReload();
}

void UProjectileShooterComponent::StartReload()
{
	if (!HasMaximumAmmo())
	{
		if (!ReloadTimerHandle.IsValid())
		{
			bool bIsTimerManagerValid;
			FTimerManager& TimerManager = GetTimerManager(bIsTimerManagerValid);
			if (bIsTimerManagerValid)
			{
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, FName("Reload"), 1);
				TimerManager.SetTimer(ReloadTimerHandle, TimerDelegate, ReloadCooldownInSeconds, false);
			}
		}
	}
}

FTimerManager& UProjectileShooterComponent::GetTimerManager(bool& bOutIsTimerManagerValid) const
{
	UWorld* CurrentWorld = GetWorld();
	bOutIsTimerManagerValid = CurrentWorld->IsValidLowLevel();
	FTimerManager& TimerManager = CurrentWorld->GetTimerManager();
	return TimerManager;
}

bool UProjectileShooterComponent::Shoot()
{
	bool bHasAmmo = HasAmmo();
	if (bHasAmmo)
	{
		UWorld* CurrentWorld = GetWorld();
		if (CurrentWorld->IsValidLowLevel())
		{
			APawn* ComponentOwner = Cast<APawn>(GetOwner());
			if (ComponentOwner->IsValidLowLevel())
			{
				AActor* SpawnedProjectile = CurrentWorld->SpawnActor<AActor>(ProjectileClass, ComponentOwner->GetActorLocation(), ComponentOwner->GetControlRotation());
				--CurrentAmmo;
				StartReload();
			}
		}
	}
	return bHasAmmo;
}

int32 UProjectileShooterComponent::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

bool UProjectileShooterComponent::HasAmmo() const
{
	return (CurrentAmmo > 0);
}

bool UProjectileShooterComponent::HasMaximumAmmo() const
{
	return (CurrentAmmo >= MaximumAmmo);
}

void UProjectileShooterComponent::Reload(int32 AmountOfAmmoToReload)
{
	int32 NewAmmoAfterReload = CurrentAmmo + AmountOfAmmoToReload;
	if (NewAmmoAfterReload > MaximumAmmo)
	{
		NewAmmoAfterReload = MaximumAmmo;
	}
	CurrentAmmo = NewAmmoAfterReload;
	bool bIsTimerManagerValid;
	FTimerManager& TimerManager = GetTimerManager(bIsTimerManagerValid);
	if (bIsTimerManagerValid)
	{
		TimerManager.ClearTimer(ReloadTimerHandle);
	}
	StartReload();
}

float UProjectileShooterComponent::GetRemainingReloadCooldownInSeconds() const
{
	float SecondsRemaining = 0.f;
	bool bIsTimerManagerValid;
	FTimerManager& TimerManager = GetTimerManager(bIsTimerManagerValid);
	if (bIsTimerManagerValid)
	{
		SecondsRemaining = TimerManager.GetTimerRemaining(ReloadTimerHandle);
		if (SecondsRemaining < 0.f)
		{
			SecondsRemaining = 0.f;
		}
	}
	return SecondsRemaining;
}
