// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Shootables/Shootable.h"

// Add default functionality here for any IShootable functions that are not pure virtual.

float IShootable::GetAutoAimPriority_Implementation() const
{
	return 0.f;
}
