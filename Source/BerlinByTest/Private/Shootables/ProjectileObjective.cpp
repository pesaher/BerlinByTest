// Fill out your copyright notice in the Description page of Project Settings.

#include "Shootables/ProjectileObjective.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AProjectileObjective::AProjectileObjective()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	// Set the root component to be the mesh component
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void AProjectileObjective::BeginPlay()
{
	Super::BeginPlay();
}

float AProjectileObjective::GetAutoAimPriority_Implementation() const
{
	return AutoAimPriority;
}
