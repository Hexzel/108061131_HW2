# 108061131_HW2

Items:
	resistos: 4.6 kohm
	capacitor: 4.7 nF * 3 (in parallel)

Cut off frequency:
	245 Hz

Pin set:
	D9: down button
	D10: up botton
	D11: confirm button
	D7: generate waveform
	A0: receive waveform

How to use:
	board:
		press up or down to slect frequency
		press comfirm to generate wave
		press comfirm again to send data to PC
	
	PC (plot.py):
		excute plot.py
		press comfirm and send data to PC
		The code will produce two graph. 
		The upper one is waveform and the lower one is FFT result.
