class SchanaModPartyMarkerSettings  extends Managed {
    protected static string DIR = "$profile:SchanaModParty";
    protected static string PATH = DIR + "\\markers.json";

    protected autoptr array<vector> markers;

    void SchanaModPartyMarkerSettings () {
        markers = new array<vector> ();
    }

    void Add (vector mark) {
        markers.Insert (mark);
        Save ();
    }

    void Clear () {
        markers.Clear ();
        Save ();
    }

    array<vector> GetMarkers () {
        auto response = new array<vector> ();

        for (int i = 0; i < markers.Count (); ++i) {
            response.Insert (markers.Get (i));
        }

        return response;
    }

    void Save () {
        if (GetGame ().IsClient ()) {
            if (!FileExist (DIR)) {
                MakeDirectory (DIR);
            }
            JsonFileLoader<SchanaModPartyMarkerSettings>.JsonSaveFile (PATH, this);
        }
    }

    static SchanaModPartyMarkerSettings Get () {
        auto settings = new SchanaModPartyMarkerSettings ();

        if (FileExist (PATH)) {
            JsonFileLoader<SchanaModPartyMarkerSettings>.JsonLoadFile (PATH, settings);
        }

        return settings;
    }
}

static ref SchanaModPartyMarkerSettings g_SchanaPartyMarkerSettings;
static SchanaModPartyMarkerSettings GetSchanaPartyMarkerSettings () {
    if (g_Game.IsClient () && !g_SchanaPartyMarkerSettings) {
        g_SchanaPartyMarkerSettings = SchanaModPartyMarkerSettings.Get ();
        g_SchanaPartyMarkerSettings.Save ();
    }
    return g_SchanaPartyMarkerSettings;
}