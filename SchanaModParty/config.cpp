class CfgPatches
{
	class SchanaModParty
	{
		requiredAddons[] = { "SchanaModPartyDefine", "JM_CF_Scripts" };
		units[] = {};
		weapons[] = {};
	};
};

class CfgMods
{
	class SchanaModParty
	{
		name = "SchanaModParty";
		action = "https://github.com/schana/dayz-mod-party";
		author = "schana";
		type = "mod";
		inputs = "SchanaModParty/Data/Inputs.xml";
		dependencies[] =
		{
			"Game",
			"Mission"
		};
		class defs
		{
			class widgetStyles
			{
				files[] = {"SchanaModParty/gui/looknfeel/prefabs.styles"};
			};
			class imageSets
			{
				files[] = {"SchanaModParty/gui/imagesets/imageset.imageset","SchanaModParty/gui/imagesets/prefabs.imageset"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {
					"SchanaModParty/3_Game"
				};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {
					"SchanaModParty/5_Mission"
				};
			};
		};
	};
};
