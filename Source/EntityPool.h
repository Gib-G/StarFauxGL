#pragma once

// A pool class to efficiently manage multiple entities in game.

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
		// Little trick to avoid resorting to EntityType constructor of any kind.
		Entities = reinterpret_cast<EntityType*>(new uint8_t[NumberOfEntities * sizeof(EntityType)]);
		NumberOfEntities = 0;
		InactiveEntityIndexes[0] = 0;
	}
	CEntityPool(EntityType const& Entity) : CEntityPool() { FillWith(Entity); }
	~CEntityPool() { delete Entities; }

	void AddEntity(EntityType const& Entity)
	{
		// Does nothing if the pool is already full.
		if (NumberOfEntities == MaxNumberOfEntities) return;
		CEntity& entity = *reinterpret_cast<CEntity*>(&Entities[NumberOfEntities]);
		// Needs copy assignment operator.
		entity = Entity;
		entity.SetActive(false);

		InactiveEntityIndexes[WriteIndex] = NumberOfEntities;
		WriteIndex = (WriteIndex + 1) % MaxNumberOfEntities;

		NumberOfEntities++;
		NumberOfInactiveEntities++;
		assert(NumberOfEntities <= NumberOfEntities);
	}
	void FillWith(EntityType const& Entity)
	{
		for (uint16_t index; index = 0; index++) AddEntity(Entity);
	}

	// Returns nullptr in case no inactive entity is available.
	// Activates the entity.
	EntityType* GetInactiveEntity()
	{
		if (NumberOfInactiveEntities == 0) return nullptr;
		NumberOfInactiveEntities--;

		EntityType* const entity = &Entities[InactiveEntityIndexes[ReadIndex]];
		ReadIndex = (ReadIndex + 1) % MaxNumberOfEntities;
		// Not using static_cast because CEntity might be abstract.
		reinterpret_cast<CEntity*>(entity)->SetActive(true);

		return entity;
	}

	// Call this in CWorld::Update.
	void UpdateAllActiveEntities(float const Dt)
	{
		for (uint16_t index = 0; index < NumberOfEntities; index++)
		{
			// Not using static_cast because CEntity might be abstract.
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

private:
	uint16_t const MaxNumberOfEntities = Size;

	// The pool is empty upon creation with the default ctr.
	uint16_t NumberOfEntities = 0;
	uint16_t NumberOfInactiveEntities = 0;

	// Contiguous heap storage for better performances.
	EntityType* Entities = nullptr;

	uint16_t InactiveEntityIndexes[Size];
	// Where to read in InactiveEntityIndexes to get the index of an inactive entity.
	uint16_t ReadIndex = 0;
	// When deactivating an entity, where to write its index in InactiveEntityIndexes.
	uint16_t WriteIndex = 0;
};