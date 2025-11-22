// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/ColorMappingData.h"

FColorMapping UColorMappingData::GetColorMapping(EFlashlightColor Color)
{
	for (auto& Mapping : ColorMappings)
	{
		if (Mapping.Color == Color)
		{
			return Mapping;
		}
	}
	return FColorMapping();
}
