#ifdef BASICMAP

class SchanaPartyBasicMapMarkerBaseFactory extends BasicMapMarkerFactory {
    override BasicMapMarker Create (string name, vector position) {
        BasicMapMarker marker = super.Create (name, position);
        marker.SetIcon ("BasicMap\\gui\\images\\location.paa");
        marker.Colour.Set (0, 28);
        marker.Colour.Set (1, 234);
        marker.Colour.Set (2, 245);
        return marker;
    }
}

class SchanaPartyLocalMarkerFactory extends SchanaPartyBasicMapMarkerBaseFactory {
    override BasicMapMarker Create (string name, vector position) {
        BasicMapMarker marker = super.Create (name, position);
        marker.SetGroup (SchanaPartyBasicMapAPI.GROUP_ID_LOCAL);
        return marker;
    }
}

class SchanaPartyRemoteMarkerFactory extends SchanaPartyBasicMapMarkerBaseFactory {
    override BasicMapMarker Create (string name, vector position) {
        BasicMapMarker marker = super.Create (name, position);
        marker.SetGroup (SchanaPartyBasicMapAPI.GROUP_ID_REMOTE);
        return marker;
    }
}

class SchanaPartyMemberMarkerFactory extends SchanaPartyBasicMapMarkerBaseFactory {
    override BasicMapMarker Create (string name, vector position) {
        BasicMapMarker marker = super.Create (name, position);
        marker.SetGroup (SchanaPartyBasicMapAPI.GROUP_ID_MEMBERS);
        return marker;
    }
}

#endif