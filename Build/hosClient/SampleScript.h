#pragma once

#include "hosEngine/com_client.h"

class SampleScript : public hos::com::Script
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;
};