#pragma once

namespace Offset
{
	// SE 1.5.97: Scaleform RVAs verified against the installed 2.2.1 DLL/PDB.
	// These vtables do not all have Address Library IDs. Query rejects other runtimes.
	namespace GASActionBufferData
	{
		inline constexpr REL::Offset Vtbl(0x17BC3F0);
	}

	namespace GASDoAction
	{
		inline constexpr REL::ID Vtbl(291613);
	}

	namespace GFxInitImportActions
	{
		inline constexpr REL::Offset Vtbl(0x17DC4C8);
	}

	namespace GFxMovieDefImpl
	{
		inline constexpr REL::Offset Vtbl(0x17DD860);
	}

	namespace GFxPlaceObject2
	{
		inline constexpr REL::Offset Vtbl(0x17BE0E0);
	}

	namespace GFxRemoveObject2
	{
		inline constexpr REL::Offset Vtbl(0x17DC448);
	}

	namespace HUDMenu
	{
		inline constexpr REL::ID Ctor(50716);
	}

	namespace HUDNotifications
	{
		inline constexpr REL::ID ProcessMessage(50758);
	}

	namespace LocalMapMenu
	{
		inline constexpr REL::ID PopulateData(52081);
	}

	namespace MapMenu
	{
		inline constexpr REL::ID Ctor(52206);
	}
}
