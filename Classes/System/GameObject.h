#pragma once

struct CGameObject : CObject
{
	IL2CPP_PAD(0x20);			// 0x28
	void* m_ComponentList;		// 0x30
	IL2CPP_PAD(0x8);
	int m_iComponents;			// 0x40
	IL2CPP_PAD(0x16);
	bool m_active;				// 0x56
	bool m_activeCached;		// 0x57
	IL2CPP_PAD(0x2);
	char* m_ObjectName;			// 0x60

	CTransform* GetTransform()
	{
		CGameObject gameObject;
		um::ReadMemory(this, &gameObject);

		void* buffer = malloc(gameObject.m_iComponents * sizeof(ComponentList));

		if (!buffer)
		{
			return nullptr;
		}

		um::ReadMemory((void*)((uintptr_t)gameObject.m_ComponentList + 8), buffer, gameObject.m_iComponents * sizeof(ComponentList));

		ComponentList* list = (ComponentList*)buffer;

		CComponent component;
		um::ReadMemory(list[0].m_Component, &component);

		free(buffer);

		return component.m_Transform;
	}

	/*
		System Class has differen Offset, make sure which component you are touching.
	*/
	CComponent* GetComponentByIndex(int index, bool system_class = false)
	{
		CGameObject gameObject;
		um::ReadMemory(this, &gameObject);

		void* buffer = malloc(gameObject.m_iComponents * sizeof(ComponentList));
		um::ReadMemory((void*)((uintptr_t)gameObject.m_ComponentList + 8), buffer, gameObject.m_iComponents * sizeof(ComponentList));


		ComponentList* list = (ComponentList*)buffer;

		CComponent component;
		if (system_class)
		{
			CComponent* com = list[index].m_Component;
			free(buffer);
			return com;
		}
		else
		{
			um::ReadMemory(list[index].m_Component, &component);
		}
		free(buffer);

		return component.m_Component;
	}


	/*
		This is not finished yet, if you use it, you need to know the Component if its Unity System Class.
	*/
	CComponent* ComponentIterator(std::function<CComponent* (void*)> loop_func)
	{
		CGameObject gameObject;
		um::ReadMemory(this, &gameObject);

		void* buffer = malloc(gameObject.m_iComponents * sizeof(ComponentList));
		um::ReadMemory((void*)((uintptr_t)gameObject.m_ComponentList + 8), buffer, gameObject.m_iComponents * sizeof(ComponentList));

		for (int i = 0; i < gameObject.m_iComponents; i++)
		{
			ComponentList* list = (ComponentList*)buffer;

			CComponent component;
			um::ReadMemory(list[i].m_Component, &component);

			if (loop_func(component.m_Component))
			{
				free(buffer);
				return component.m_Component;
			}

		}
		free(buffer);
		return nullptr;
	}
};