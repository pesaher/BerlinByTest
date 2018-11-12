// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles/ProjectileShooterComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Projectiles/Shootable.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UProjectileShooterComponent::UProjectileShooterComponent()
{
	MaximumAmmo = 6;
	InitialAmmo = MaximumAmmo;
	ReloadCooldownInSeconds = 5.f;
	PriorityWeight = 0.f;
	DistanceWeight = 0.f;
	MaximumDistance = -1.f;
	FocusWeight = 1.f;
	MaximumVisionAngle = 30.f;
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

const AActor* UProjectileShooterComponent::GetCenteredShootableActor() const
{
	const AActor* CenteredShootableActor = nullptr;
	const APawn* const ComponentOwner = Cast<APawn>(GetOwner());
	if (ComponentOwner->IsValidLowLevel())
	{
		float CurrentMaximumAutoAimScore = 0.f;
		float CosineOfMaximumVisionAngle = FGenericPlatformMath::Cos(FMath::DegreesToRadians(MaximumVisionAngle));
		FVector OwnerLocation = ComponentOwner->GetActorLocation();
		FVector OwnerForwardVector = UKismetMathLibrary::GetForwardVector(ComponentOwner->GetControlRotation());
		OwnerForwardVector.Normalize();
		UWorld* const CurrentWorld = GetWorld();
		TArray<AActor*> ShootableActors;
		UGameplayStatics::GetAllActorsWithInterface(this, UShootable::StaticClass(), ShootableActors);
		for (const AActor* ShootableActor : ShootableActors)
		{
			FVector ShootableActorLocation = ShootableActor->GetActorLocation();
			FVector VectorToShootable = ShootableActorLocation - OwnerLocation;
			float DistanceToShootable = VectorToShootable.Size();
			if ((MaximumDistance <= 0.f) || DistanceToShootable < MaximumDistance))
			{
				VectorToShootable.Normalize();
				float DotProductOfVectors = FVector::DotProduct(VectorToShootable, OwnerForwardVector);
				if (DotProductOfVectors > CosineOfMaximumVisionAngle)
				{
					FHitResult TraceHit;
					CurrentWorld->LineTraceSingleByChannel(TraceHit, OwnerLocation, ShootableActorLocation, ECC_GameTraceChannel2);
					if (TraceHit.GetActor() == ShootableActor)
					{
						float ShootableActorAutoAimScore = GetAutoAimScore(0.f, DistanceToShootable, DotProductOfVectors, CosineOfMaximumVisionAngle);
						if (ShootableActorAutoAimScore > CurrentMaximumAutoAimScore)
						{
							CenteredShootableActor = ShootableActor;
							CurrentMaximumAutoAimScore = ShootableActorAutoAimScore;
						}
					}
				}
			}
		}
	}
	return CenteredShootableActor;
}

float UProjectileShooterComponent::GetAutoAimScore(float InPriority, float InDistance, float InCosineOfVisionAngle, float InCosineOfMaximumVisionAngle) const
{
	float AutoAimScore = 0.f;
	float DistanceFraction = 0.f;
	float TotalPrioritySum = PriorityWeight + FocusWeight;
	if (MaximumDistance > 0.f)
	{
		TotalPrioritySum += DistanceWeight;
		DistanceFraction = InDistance * DistanceWeight / MaximumDistance;
	}
	if (TotalPrioritySum > 0.f)
	{
		float PriorityFraction = InPriority * PriorityWeight * 0.1f;
		float FocusFraction = InCosineOfVisionAngle * FocusWeight / InCosineOfMaximumVisionAngle;
		float SumOfFractions = PriorityFraction + FocusFraction + DistanceFraction;
		AutoAimScore = SumOfFractions / TotalPrioritySum;
	}
	return AutoAimScore;
}

bool UProjectileShooterComponent::Shoot()
{
	bool bHasAmmo = HasAmmo();
	if (bHasAmmo)
	{
		UWorld* const CurrentWorld = GetWorld();
		if (CurrentWorld->IsValidLowLevel())
		{
			const APawn* const ComponentOwner = Cast<APawn>(GetOwner());
			if (ComponentOwner->IsValidLowLevel())
			{
				FRotator ProjectileRotation = { 0.f, ComponentOwner->GetControlRotation().Yaw, 0.f };
				FVector ProjectileLocation = ComponentOwner->GetActorLocation();
				const AActor* const CenteredShootableActor = GetCenteredShootableActor();
				if (CenteredShootableActor->IsValidLowLevel())
				{
					ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(ProjectileLocation, CenteredShootableActor->GetActorLocation());
				}
				CurrentWorld->SpawnActor<AActor>(ProjectileClass, ProjectileLocation, ProjectileRotation);
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
