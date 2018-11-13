// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectiles/ProjectileActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Shootables/Shootable.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
	InitialLifeSpan = 5.f;
	// Create a sphere collision component for the projectile
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	// Set the sphere's collision radius.
	CollisionComponent->InitSphereRadius(15.0f);
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileActor::BeginHit);
	// Set the root component to be the collision component.
	RootComponent = CollisionComponent;
	// Add a mesh to the projectile
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	MeshComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	// Set up the projectile movement
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileActor::BeginHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Notify the other actor that it has been hit by a projectile
	if (OtherActor->GetClass()->ImplementsInterface(UShootable::StaticClass()))
	{
		IShootable::Execute_ProjectileHit(OtherActor);
	}
	// Destroy the projectile whenever it hits another actor
	Destroy();
}
