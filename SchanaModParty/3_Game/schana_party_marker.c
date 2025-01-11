class SchanaPartyMarkerMenu extends UIScriptedMenu {
    static string SCHANA_PARTY_MARKER_DELETE = "SCHANA_PARTY_MARKER_DELETE";

    protected Widget m_SchanaPartyMarkerRoot;
    protected TextWidget m_SchanaPartyMarkerNametag;
    protected TextWidget m_SchanaPartyMarkerDistance;
    protected ImageWidget m_SchanaPartyMarkerIcon;

    protected string m_SchanaPartyMarkerName = "";
    protected vector m_SchanaPartyMarkerPosition = "0 0 0";

    void SchanaPartyMarkerMenu (string name, vector position) {
        m_SchanaPartyMarkerRoot = Widget.Cast(GetGame ().GetWorkspace ().CreateWidgets ("SchanaModParty/GUI/Layouts/marker.layout"));
        m_SchanaPartyMarkerNametag = TextWidget.Cast (m_SchanaPartyMarkerRoot.FindAnyWidget ("nametag"));
        m_SchanaPartyMarkerDistance = TextWidget.Cast (m_SchanaPartyMarkerRoot.FindAnyWidget ("distance"));
        m_SchanaPartyMarkerIcon = ImageWidget.Cast (m_SchanaPartyMarkerRoot.FindAnyWidget ("icon"));
        m_SchanaPartyMarkerRoot.Show (false);

        m_SchanaPartyMarkerName = name;
        m_SchanaPartyMarkerPosition = position;

        GetGame ().GetCallQueue (CALL_CATEGORY_GUI).CallLater (this.SchanaUpdate, 16, true);
    }

    void ~SchanaPartyMarkerMenu () {
        GetGame ().GetCallQueue (CALL_CATEGORY_GUI).Remove (this.SchanaUpdate);
        if (m_SchanaPartyMarkerRoot != null) {
            m_SchanaPartyMarkerRoot.Show (false);
            m_SchanaPartyMarkerRoot.Unlink ();
        }
    }

    void SchanaUpdate () {
        if (SchanaPartyMarkerShouldShow ()) {
            SchanaPartyMarkerUpdate ();
        } else if (m_SchanaPartyMarkerRoot != null) {
            m_SchanaPartyMarkerRoot.Show (false);
            delete this;
        }
    }

    protected bool SchanaPartyMarkerShouldShow () {
        if (m_SchanaPartyMarkerRoot == null) {
            return false;
        }
        if (!GetGame ().GetPlayer ()) {
            return false;
        }
        if (m_SchanaPartyMarkerName == SCHANA_PARTY_MARKER_DELETE) {
            return false;
        }
        return true;
    }

    void SchanaPartySetRemoveFlag () {
        m_SchanaPartyMarkerName = SCHANA_PARTY_MARKER_DELETE;
    }

    void SchanaPartyMarkerUpdate () {
        float x, y, distance;
        vector position = m_SchanaPartyMarkerPosition;
        vector screenPosition = GetGame ().GetScreenPos (position + "0 0.2 0");

        x = Math.Round (screenPosition[0]);
        y = Math.Round (screenPosition[1]);
        m_SchanaPartyMarkerRoot.SetPos (x, y);

        distance = Math.Round (vector.Distance (position, GetGame ().GetPlayer ().GetPosition ()));
        string distanceString = distance.ToString () + "m";
        if (distance > 1000) {
            distanceString = (Math.Round (distance / 100) / 10).ToString () + "km";
        } else {
            distanceString = "";
        }
        m_SchanaPartyMarkerNametag.SetText (m_SchanaPartyMarkerName);
        m_SchanaPartyMarkerDistance.SetText (distanceString);

        m_SchanaPartyMarkerRoot.Show (SchanaPartyMarkerVisibleOnScreen ());
    }

    protected bool SchanaPartyMarkerVisibleOnScreen () {
        vector position = m_SchanaPartyMarkerPosition;
        vector screenPositionRelative = GetGame ().GetScreenPosRelative (position);

        if (screenPositionRelative[0] >= 1 || screenPositionRelative[0] == 0 || screenPositionRelative[1] >= 1 || screenPositionRelative[1] == 0) {
            return false;
        } else if (screenPositionRelative[2] < 0) {
            return false;
        }
        return true;
    }
}