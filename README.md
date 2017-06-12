# AFDN-reverb
(High-Order) Ambisonics Feedback Delay Network Reverb

## Organization (see wiki for more info)

## Max:
- Max/lib/ : Directory containing abstractions and subpatches used in Max implementation of reverb effect. Users may refer to these, but they need not be opened to run the effect.
- Max/TestingEnvironments/reverb-rotation-test-FDN.maxpat : Patch for listening to sound sources through the reverb effect. Uses IRCAM Spat objects for metering, encoding, and UI elements. These objects may be removed for uses without access to Spat.
- Max/TestingEnvironemts/FAUST-test-FDN.maxpat : Patch for listening to reverb effect running Faust implementation as a Faustgen~ object
- Max/TestingEnvironemts/AFDN-Binaural.maxpat : Patch to create binaural renderings from the ouput of the reverb effect in ambisonics

## FAUST:
- Source/FAUST/AFDN.lib: A library file containing all modules necessary to run the AFDN reverb algorithm.
- Source/FAUST/afdnRev1st.dsp: Faust effect demonstrating the full reverb effect on an 1st order ambisonics input. Can be compiled in command line or run in Max using the fausgen~ object.
- Source/FAUST/afdnRev2nd.dsp: Same as above for 2nd order ambisonics
- Source/FAUST/afdnRev3rd.dsp: Same as above for 3rd order ambisonics

## Audio Demo:
- Max/TestingEnvironemts/Binaural-AFDN.wav : Quintessential CCRMA electric guitar sample encoded in 3rd order
ambisonics and rendered to binaural for headphone listening. See wiki/Discussion for more details.
