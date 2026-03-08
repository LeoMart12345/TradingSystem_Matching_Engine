import socket
import struct
import threading
import time
from collections import deque
from datetime import datetime

import dash
from dash import dcc, html
from dash.dependencies import Input, Output
import plotly.graph_objects as go

# shared state
market_data = {
    "bestBid": 0,
    "bestAsk": 0,
    "bidVolume": 0,
    "askVolume": 0,
}

MAX_HISTORY = 200
price_history = deque(maxlen=MAX_HISTORY)   # (timestamp, mid_price)
spread_history = deque(maxlen=MAX_HISTORY)  # (timestamp, spread)


# UDP multicast receiver
def udp_receiver():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", 9999))

    mreq = struct.pack(
        "4sL",
        socket.inet_aton("239.0.0.1"),
        socket.INADDR_ANY,
    )
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    while True:
        try:
            data, _ = sock.recvfrom(1024)
            parts = data.decode().strip().split(",")
            if len(parts) >= 4:
                bid = int(parts[0])
                ask = int(parts[1])
                market_data["bestBid"]    = bid
                market_data["bestAsk"]    = ask
                market_data["bidVolume"]  = int(parts[2])
                market_data["askVolume"]  = int(parts[3])

                if bid > 0 and ask > 0:
                    now = datetime.now()
                    price_history.append((now, (bid + ask) / 2))
                    spread_history.append((now, ask - bid))
        except Exception:
            pass


threading.Thread(target=udp_receiver, daemon=True).start()

# colour palette
BG        = "#1d1d1e"
PANEL     = "#626161"
BORDER    = "#1c2535"
BID_CLR   = "#00e5a0"
ASK_CLR   = "#ff4560"
ACCENT    = "#4d9fff"
TEXT      = "#e5ecf7"
DIM       = "#ecedf0"
FONT_MONO = "'JetBrains Mono', 'Fira Code', monospace"
FONT_UI   = "'DM Sans', sans-serif"

def card(children, style=None):
    base = {
        "background": PANEL,
        "border": f"1px solid {BORDER}",
        "borderRadius": "4px",
        "padding": "20px",
    }
    if style:
        base.update(style)
    return html.Div(children, style=base)


# layout
app = dash.Dash(
    __name__,
    external_stylesheets=[
        "https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;600&family=DM+Sans:wght@300;400;500&display=swap"
    ],
)

app.layout = html.Div(
    style={
        "background": BG,
        "minHeight": "100vh",
        "fontFamily": FONT_UI,
        "color": TEXT,
        "padding": "24px",
        "boxSizing": "border-box",
    },
    children=[
        #header
        html.Div(
            style={
                "display": "flex",
                "alignItems": "baseline",
                "gap": "16px",
                "marginBottom": "24px",
                "borderBottom": f"1px solid {BORDER}",
                "paddingBottom": "16px",
            },
            children=[
                html.Span(
                    "TSLA",
                    style={
                        "fontFamily": FONT_MONO,
                        "fontSize": "22px",
                        "fontWeight": "600",
                        "color": ACCENT,
                        "letterSpacing": "0.12em",
                    },
                ),
                html.Span(
                    "Matching Engine — Live Feed",
                    style={"fontSize": "13px", "color": DIM, "letterSpacing": "0.05em"},
                ),
                html.Span(id="clock", style={"marginLeft": "auto", "fontFamily": FONT_MONO, "fontSize": "12px", "color": DIM}),
            ],
        ),

        # top row: bid/ask cards + spread
        html.Div(
            style={"display": "grid", "gridTemplateColumns": "1fr 1fr 1fr", "gap": "12px", "marginBottom": "12px"},
            children=[
                card([
                    html.Div("BEST BID", style={"fontSize": "10px", "letterSpacing": "0.15em", "color": DIM, "marginBottom": "8px"}),
                    html.Div(id="bid-price", style={"fontFamily": FONT_MONO, "fontSize": "36px", "fontWeight": "600", "color": BID_CLR}),
                    html.Div(id="bid-vol",   style={"fontFamily": FONT_MONO, "fontSize": "12px", "color": DIM, "marginTop": "6px"}),
                ]),
                card([
                    html.Div("BEST ASK", style={"fontSize": "10px", "letterSpacing": "0.15em", "color": DIM, "marginBottom": "8px"}),
                    html.Div(id="ask-price", style={"fontFamily": FONT_MONO, "fontSize": "36px", "fontWeight": "600", "color": ASK_CLR}),
                    html.Div(id="ask-vol",   style={"fontFamily": FONT_MONO, "fontSize": "12px", "color": DIM, "marginTop": "6px"}),
                ]),
                card([
                    html.Div("SPREAD / MID", style={"fontSize": "10px", "letterSpacing": "0.15em", "color": DIM, "marginBottom": "8px"}),
                    html.Div(id="spread",    style={"fontFamily": FONT_MONO, "fontSize": "36px", "fontWeight": "600", "color": ACCENT}),
                    html.Div(id="mid-price", style={"fontFamily": FONT_MONO, "fontSize": "12px", "color": DIM, "marginTop": "6px"}),
                ]),
            ],
        ),

        # ── bottom row: mid price chart + spread chart ──
        html.Div(
            style={"display": "grid", "gridTemplateColumns": "2fr 1fr", "gap": "12px"},
            children=[
                card([
                    html.Div("MID PRICE", style={"fontSize": "10px", "letterSpacing": "0.15em", "color": DIM, "marginBottom": "8px"}),
                    dcc.Graph(id="price-chart", config={"displayModeBar": False},
                              style={"height": "260px"}),
                ], style={"padding": "20px 20px 8px"}),

                card([
                    html.Div("SPREAD HISTORY", style={"fontSize": "10px", "letterSpacing": "0.15em", "color": DIM, "marginBottom": "8px"}),
                    dcc.Graph(id="spread-chart", config={"displayModeBar": False},
                              style={"height": "260px"}),
                ], style={"padding": "20px 20px 8px"}),
            ],
        ),

        dcc.Interval(id="tick", interval=500, n_intervals=0),
    ],
)


# chart helpers
CHART_LAYOUT = dict(
    paper_bgcolor="rgba(0,0,0,0)",
    plot_bgcolor="rgba(0,0,0,0)",
    margin=dict(l=40, r=10, t=4, b=30),
    xaxis=dict(showgrid=False, color=DIM, tickfont=dict(family=FONT_MONO, size=10)),
    yaxis=dict(showgrid=True, gridcolor=BORDER, color=DIM, tickfont=dict(family=FONT_MONO, size=10)),
    font=dict(family=FONT_UI, color=TEXT),
)


def make_line_fig(xs, ys, color, fill=True):
    fig = go.Figure()
    fig.add_trace(go.Scatter(
        x=xs, y=ys,
        mode="lines",
        line=dict(color=color, width=2),
        fill="tozeroy" if fill else "none",
        fillcolor="rgba(77, 159, 255, 0.06)" if fill else "rgba(0,0,0,0)",
    ))
    fig.update_layout(**CHART_LAYOUT)
    return fig

# callback
@app.callback(
    Output("bid-price",    "children"),
    Output("ask-price",    "children"),
    Output("bid-vol",      "children"),
    Output("ask-vol",      "children"),
    Output("spread",       "children"),
    Output("mid-price",    "children"),
    Output("price-chart",  "figure"),
    Output("spread-chart", "figure"),
    Output("clock",        "children"),
    Input("tick",          "n_intervals"),
)
def refresh(_):
    bid  = market_data["bestBid"]
    ask  = market_data["bestAsk"]
    bvol = market_data["bidVolume"]
    avol = market_data["askVolume"]

    spread = ask - bid if bid and ask else 0
    mid    = (bid + ask) / 2 if bid and ask else 0

    # price chart
    if price_history:
        px_times = [p[0] for p in price_history]
        px_vals  = [p[1] for p in price_history]
        price_fig = make_line_fig(px_times, px_vals, ACCENT)
    else:
        price_fig = make_line_fig([], [], ACCENT)

    # spread chart
    if spread_history:
        sp_times = [s[0] for s in spread_history]
        sp_vals  = [s[1] for s in spread_history]
        spread_fig = make_line_fig(sp_times, sp_vals, ASK_CLR, fill=False)
    else:
        spread_fig = make_line_fig([], [], ASK_CLR, fill=False)

    clock = datetime.now().strftime("%H:%M:%S")

    return (
        f"{bid:,}" if bid else "—",
        f"{ask:,}" if ask else "—",
        f"vol {bvol:,}",
        f"vol {avol:,}",
        f"{spread:+}" if spread else "—",
        f"mid {mid:.1f}" if mid else "—",
        price_fig,
        spread_fig,
        clock,
    )


if __name__ == "__main__":
    print("Dashboard running at http://localhost:8050")
    app.run(debug=False, host="0.0.0.0", port=8050)
