// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFL_AICrowdManagement.h"

void UBPFL_AICrowdManagement::SetSeparationWeight(UCrowdFollowingComponent* inCrowdFolowingComponent, int32 inWeight)
{
	if(inCrowdFolowingComponent)
	{
		inCrowdFolowingComponent->SetCrowdSeparationWeight(inWeight, true);
		inCrowdFolowingComponent->SetCrowdSeparation(true, true); //Just in case it is not enabled.
	}
}