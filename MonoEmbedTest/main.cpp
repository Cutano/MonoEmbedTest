#include <iostream>
#include <set>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>

class Component
{
public:
	int id;
	MonoString* tag;

	Component(int id, MonoString* tag) : id(id), tag(tag) {
		std::cout << "Component #" << id << " constructed in native code!" << std::endl;
	}

	~Component() {
		std::cout << "Component #" << id << " deconstructed in native code!" << std::endl;
	}
};

std::set<Component*> componentPool;

MonoClassField* nativeHandleField;
MonoDomain* domain;
MonoClass* componentClass;

int ManagedCallComponentGetId(const Component* component) {
	return component->id;
}

MonoString* ManagedCallComponentGetTag(const Component* component) {
	return component->tag;
}

Component* ManagedCallComponentNew(int id, MonoString* tag) {
	auto newComp = new Component(id, tag);
	componentPool.insert(newComp);
	return newComp;
}

void ManagedCallComponentDelete(Component* component) {
	componentPool.erase(component);
	delete component;
}

int main() {
	const char* managedBinPath = "D:\\CppDev\\MonoEmbedTest\\ManagedCode\\Test.dll";
	
	domain = mono_jit_init(managedBinPath);
	// domain = mono_jit_init_version("Test", "v4.5");

	mono_set_dirs("..\\lib", "..\\etc");
	mono_config_parse(nullptr);

	MonoAssembly* assembly = mono_domain_assembly_open(domain, managedBinPath);
	MonoImage* image = mono_assembly_get_image(assembly);

	mono_add_internal_call("ManagedCode.Component::NativeCallGetId", reinterpret_cast<void*>(ManagedCallComponentGetId));
	mono_add_internal_call("ManagedCode.Component::NativeCallGetTagInternal", reinterpret_cast<void*>(ManagedCallComponentGetTag));
	mono_add_internal_call("ManagedCode.Component::NativeCallComponentNew", reinterpret_cast<void*>(ManagedCallComponentNew));
	mono_add_internal_call("ManagedCode.Component::NativeCallComponentDelete", reinterpret_cast<void*>(ManagedCallComponentDelete));

	componentClass = mono_class_from_name(image, "ManagedCode", "Component");
	nativeHandleField = mono_class_get_field_from_name(componentClass, "_nativeHandle");

	MonoClass* mainClass = mono_class_from_name(image, "ManagedCode", "ManagedClass");
	MonoMethodDesc* entryPointMethodDesc = mono_method_desc_new("ManagedCode.ManagedClass:Main()", true);
	MonoMethod* entryPointMethod = mono_method_desc_search_in_class(entryPointMethodDesc, mainClass);

	mono_method_desc_free(entryPointMethodDesc);

	mono_runtime_invoke(entryPointMethod, nullptr, nullptr, nullptr);

	mono_jit_cleanup(domain);

	if (componentPool.size() != 0)
	{
		for (auto comp : componentPool)
		{
			std::cout << "WARNING: Component #" << comp->id << " constructed in native code (May cause by memory leak)" << std::endl;
			delete comp;
		}

		componentPool.clear();
	}

	return 0;
}