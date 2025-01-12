#ifdef BASICMAP
modded class BasicMapController {
    override void OnMarkerSave (ref BasicMapMarker marker) {
        switch (marker.GetGroup ()) {
            case SchanaPartyBasicMapAPI.GROUP_ID_LOCAL:
                SchanaPartyBasicMapAPI.Get ().SendCreatedMarker (marker);
                break;
            default:
                break;
        }
        super.OnMarkerSave (marker);
    }

    override void OnMarkerDelete (ref BasicMapMarker marker) {
        switch (marker.GetGroup ()) {
            case SchanaPartyBasicMapAPI.GROUP_ID_LOCAL:
                SchanaPartyBasicMapAPI.Get ().SendDeletedMarker (marker);
                break;
            default:
                break;
        }
        super.OnMarkerDelete (marker);
    }
}
#endif