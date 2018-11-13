// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles/ProjectileShooterComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Shootables/Shootable.h"
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
	// In case the player doesn't start with full ammo, we try to start a reload cooldown
	StartReload();
}

void UProjectileShooterComponent::StartReload()
{
	// If the player doesn't have his ammo already full...
	if (!HasMaximumAmmo())
	{
		// ... and if there is no reload cooldown currently running...
		if (!ReloadTimerHandle.IsValid())
		{
			// ... a new reload cooldown is started
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
		//Get all actors that implement the shootable interface to see which one is auto-aimable
		TArray<AActor*> ShootableActors;
		UGameplayStatics::GetAllActorsWithInterface(this, UShootable::StaticClass(), ShootableActors);
		for (AActor* ShootableActor : ShootableActors)
		{
			// Check that the actor is within the maximum distance range
			FVector ShootableActorLocation = ShootableActor->GetActorLocation();
			FVector VectorToShootable = ShootableActorLocation - OwnerLocation;
			float DistanceToShootable = VectorToShootable.Size();
			if ((MaximumDistance <= 0.f) || (DistanceToShootable < MaximumDistance))
			{
				// Check that the actor is within the selected angle of vision
				VectorToShootable.Normalize();
				float DotProductOfVectors = FVector::DotProduct(VectorToShootable, OwnerForwardVector);
				if (DotProductOfVectors > CosineOfMaximumVisionAngle)
				{
					// Check that the actor does not have any other actor occluding it
					FHitResult TraceHit;
					CurrentWorld->LineTraceSingleByChannel(TraceHit, OwnerLocation, ShootableActorLocation, ECC_GameTraceChannel2);
					if (TraceHit.GetActor() == ShootableActor)
					{
						// Check if the auto-aim score of this actor is the current highest one
						if (ShootableActor->GetClass()->ImplementsInterface(UShootable::StaticClass()))
						{
							float ShootablePriority = IShootable::Execute_GetAutoAimPriority(ShootableActor);
							float ShootableActorAutoAimScore = GetAutoAimScore(ShootablePriority, DistanceToShootable, DotProductOfVectors, CosineOfMaximumVisionAngle);
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
	}
	return CenteredShootableActor;
}

float UProjectileShooterComponent::GetAutoAimScore(float InPriority, float InDistance, float InCosineOfVisionAngle, float InCosineOfMaximumVisionAngle) const
{
	float AutoAimScore = 0.f;
	float DistanceFraction = 0.f;
	float TotalPrioritySum = PriorityWeight + FocusWeight;
	// Only take distance into account if there is a maximum distance
	if (MaximumDistance > 0.f)
	{
		TotalPrioritySum += DistanceWeight;
		// Weight distance dividing it by its maximum value and taking its weight into account
		DistanceFraction = (MaximumDistance - InDistance) * DistanceWeight / MaximumDistance;
	}
	/** Don't calculate any further if it's going to divide by 0 or a negative number,
		as that messes up the calculations */
	if (TotalPrioritySum > 0.f)
	{
		// Clamp the priority between 0 and 10
		if (InPriority > 10.f)
		{
			InPriority = 10.f;
		}
		else if (InPriority < 0.f)
		{
			InPriority = 0.f;
		}
		// Weight each parameter dividing them by their maximum values and taking their weights into account
		float PriorityFraction = InPriority * PriorityWeight * 0.1f;
		float FocusFraction = (InCosineOfVisionAngle - InCosineOfMaximumVisionAngle) * FocusWeight / (1 - InCosineOfMaximumVisionAngle);
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
				/** Given that there are currently no animations, shooting in the direction of the camera
					didn't let the projectiles be clearly seen as they were occluded by the player.
					Thus, only the camera's vertical axis rotation is taken into account, so that the
					projectiles can be clearly seen from above. To revert this, just use this line instead
					of the one currently written:
					FRotator ProjectileRotation = ComponentOwner->GetControlRotation(); */
				FRotator ProjectileRotation = { 0.f, ComponentOwner->GetControlRotation().Yaw, 0.f };
				FVector ProjectileLocation = ComponentOwner->GetActorLocation();
				// If an actor can be auto-aimed to, that data is used instead
				const AActor* const CenteredShootableActor = GetCenteredShootableActor();
				if (CenteredShootableActor->IsValidLowLevel())
				{
					ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(ProjectileLocation, CenteredShootableActor->GetActorLocation());
				}
				CurrentWorld->SpawnActor<AActor>(ProjectileClass, ProjectileLocation, ProjectileRotation);
				--CurrentAmmo;
				// Try to start a new reload cooldown, since we have a free space for sure
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
	/** We invalidate the current cooldown timer, as it has already finished. Also, as this function
		can be called externaly to reload immediatly, if there was a cooldown going on it is cancelled
		so that you can't have a cooldown running with full ammo */
	bool bIsTimerManagerValid;
	FTimerManager& TimerManager = GetTimerManager(bIsTimerManagerValid);
	if (bIsTimerManagerValid)
	{
		TimerManager.ClearTimer(ReloadTimerHandle);
	}
	// We try to start a new reload cooldown after this reload has completed
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
