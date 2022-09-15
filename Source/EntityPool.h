#pragma once

// A generic pool class to efficiently manage multiple entities in game.
// (This is kinda kicking ass, ngl!)

// Copy assignment operator for EntityType has to be defined in order to fill in the pool.
// Otherwise, it won't compile...
template<typename EntityType, uint16_t Size>
class CEntityPool
{
	static_assert(std::is_base_of<CEntity, EntityType>::value);
	static_assert(Size > 0);
public:
	CEntityPool()
	{
		// Little trick to avoid resorting to any kind of EntityType constructor.
		Entities = reinterpret_cast<EntityType*>(new uint8_t[MaxNumberOfEntities * sizeof(EntityType)]);
		NumberOfEntities = 0;
		InactiveEntityIndexes[0] = 0;
	}
	CEntityPool(EntityType const& Entity) : CEntityPool() { FillWith(Entity); }
	~CEntityPool() { delete Entities; }

	void AddEntity(EntityType const& Entity)
	{
		// Does nothing if the pool is already full.
		if (NumberOfEntities == MaxNumberOfEntities) return;

		EntityType* const pEntity = &Entities[NumberOfEntities];

		// First, we just memcpy.
		// Note: This is kinda dodgy. What if the memory locations of Dst and Src overlap...
		std::memcpy(pEntity, &Entity, sizeof(EntityType));

		// Then we also call the copy assignement operator to potentially
		// do more in-depth copy work than the simple shallow copy performed by memcpy.
		// This requires EntityType and all its base classes to have trivial or user-defined copy assignment operators.
		CEntity& entity = *reinterpret_cast<CEntity*>(pEntity);
		entity = Entity; // Note: Careful with rigid body pointers management within CEntity::operator=.

		entity.SetActive(false);

		InactiveEntityIndexes[WriteIndex] = NumberOfEntities;
		WriteIndex = (WriteIndex + 1) % MaxNumberOfEntities;

		NumberOfEntities++;
		NumberOfInactiveEntities++;
		assert(NumberOfEntities <= NumberOfEntities);
	}
	void FillWith(EntityType const& Entity)
	{
		for (uint16_t index = 0; index < MaxNumberOfEntities; index++) AddEntity(Entity);
	}

	// Returns nullptr in case no inactive entity is available.
	// Activates the entity.
	EntityType* GetInactiveEntity()
	{
		if (NumberOfInactiveEntities == 0) return nullptr;
		NumberOfInactiveEntities--;

		EntityType* const entity = &Entities[InactiveEntityIndexes[ReadIndex]];
		ReadIndex = (ReadIndex + 1) % MaxNumberOfEntities;
		// Can't use static_cast because CEntity is abstract.
		reinterpret_cast<CEntity*>(entity)->SetActive(true);

		return entity;
	}

	// Call this in CWorld::Update.
	void UpdateAllActiveEntities(float const Dt)
	{
		for (uint16_t index = 0; index < NumberOfEntities; index++)
		{
			CEntity* const entity = reinterpret_cast<CEntity*>(&Entities[index]);

			// We can access entity->Active here, which is extremely dodgy since this attribute of
			// CEntity is not accessible to any of its derived classes...
			if (entity->IsActive()) entity->Update(Dt);
			if (!entity->IsActive())
			{
				if (NumberOfInactiveEntities == NumberOfEntities) continue;

				NumberOfInactiveEntities++;
				InactiveEntityIndexes[WriteIndex] = index;
				WriteIndex = (WriteIndex + 1) % MaxNumberOfEntities;
			}
		}
	}

	void DrawAllActiveEntities(glm::vec3 const& CameraPosition, glm::mat4 const& ViewMatrix, glm::mat4 const& ProjectionMatrix, glm::vec3 const& LightPosition, glm::vec3 const& LightColor)
	{
		for (uint16_t index = 0; index < NumberOfEntities; index++)
		{
			CEntity* const entity = reinterpret_cast<CEntity*>(&Entities[index]);
			if (entity->IsActive()) entity->Draw(CameraPosition, ViewMatrix, ProjectionMatrix, LightPosition, LightColor);
		}
	}

private:
	uint16_t const MaxNumberOfEntities = Size;

	// The pool is empty upon creation with the default ctr.
	uint16_t NumberOfEntities = 0;
	uint16_t NumberOfInactiveEntities = 0;

	// Contiguous heap storage for better performances (can allocate HUUGE chunks of memory tho).
	EntityType* Entities = nullptr;

	uint16_t InactiveEntityIndexes[Size];
	// Where to read in InactiveEntityIndexes to get the index of an inactive entity.
	uint16_t ReadIndex = 0;
	// When deactivating an entity, where to write its index in InactiveEntityIndexes.
	uint16_t WriteIndex = 0;
};