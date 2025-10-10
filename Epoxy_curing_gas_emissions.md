# 🔬 Epoxy Curing: Chemical Reactions and Expected Gas Emissions

RS Pro Structural Epoxy Adhesive 918-1401 is a **two-part amine-cured epoxy system**, consisting of an **epoxy resin (Part A, typically DGEBA)** and an **amine hardener (Part B)**. The curing reaction involves **epoxide rings reacting with amines to form a cross-linked polymer network**, releasing heat but minimal volatiles:

Epoxide + Amine → Cross-linked polymer + Heat

### Reaction Stages and Gas Emissions

**1. Early Stage (0–6 h, Pre-gel)**
- Epoxide and amine molecules begin initial bond formation; mixture remains liquid.
- **EtOH sensor:** Low, mostly baseline; traces only if solvents are present (~0–5 ppm).
- **VOC sensor:** Low, small emissions from unreacted monomers or additives (~1–10 ppm).
- **NH₃ sensor:** Near zero; amines largely bound.
- **BME680:** Stable readings; gas resistance slightly affected by minor VOCs.

**2. Mid Stage (6–18 h, Gelation)**
- Rapid network formation; viscosity rises; heat release peaks.
- **VOC sensor:** Significant increase as organics (aldehydes, ketones, esters, trace aromatics) are released (~10–50 ppm), marking active curing.
- **NH₃ sensor:** Begins to detect volatilised amine hardeners (~1–5 ppm), peaking slightly after VOCs.
- **EtOH sensor:** Remains minimal unless present in adhesive.
- **BME680:** Gas resistance decreases due to VOC accumulation; temperature/humidity remain near ambient.

**3. Late Stage (18–24 h, Full Cure)**
- Cross-linking completes; material becomes rigid.
- **VOC and NH₃:** Gradually plateau or decrease as curing concludes; residual volatiles trapped in polymer.
- **EtOH:** Baseline.
- **BME680:** Gas resistance stabilises; ambient temperature and humidity largely unchanged.

### Mechanistic Details
- **Epoxide-Amine Reaction:** Primary amines attack epoxide rings forming β-hydroxy amines; secondary amines react more slowly.
- **Side Reactions:** Minor hydrolysis or oxidation may produce aldehydes, ketones, or esters.
- **Amine Volatilisation:** Small fraction of low-molecular-weight amines escape into the container air, detected by NH₃ sensor.

### Summary of Sensor Expectations

| Gas | Source | Expected Trend | Typical Relative Concentration (ppm) | Sensor |
|-----|--------|----------------|--------------------------------------|--------|
| VOCs | Minor monomer evaporation, reaction by-products | Rise mid-stage, taper late | 10–50 | VOC sensor |
| NH₃ | Volatilised amine hardeners | Rise mid-stage, peak after VOC, decline late | 1–5 | NH₃ sensor |
| EtOH | Residual solvents/additives | Low, baseline | 0–5 | EtOH sensor |
| CO₂ | Trace side reactions | Minimal | <1 | Not measured |

**Notes:**  
- All runs are in a **sealed plastic container**, so gases accumulate and concentrations are higher than in open air.  
- **Room temperature** leads to gradual curing and smoother gas emission profiles.  
- Tracking **VOC rise followed by NH₃ peak** allows **non-destructive determination of curing stage**.
