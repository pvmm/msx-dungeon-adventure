; ====================================================================================================
;
; PSG Sound Driver
;
; licence:MIT Licence
; copyright-holders:Hitoshi Iwai(aburi6800)
;
; ====================================================================================================

SECTION code_user


PUBLIC SOUNDDRV_INIT
PUBLIC _sounddrv_init

PUBLIC SOUNDDRV_EXEC
PUBLIC _sounddrv_exec

PUBLIC SOUNDDRV_BGMPLAY
PUBLIC _sounddrv_bgmplay

PUBLIC SOUNDDRV_SFXPLAY
PUBLIC _sounddrv_sfxplay

PUBLIC SOUNDDRV_STOP
PUBLIC _sounddrv_stop

PUBLIC SOUNDDRV_PAUSE
PUBLIC _sounddrv_pause

PUBLIC SOUNDDRV_RESUME
PUBLIC _sounddrv_resume

PUBLIC SOUNDDRV_STATE
PUBLIC _sounddrv_state


; ====================================================================================================
; DRIVER INITIALIZE
; ====================================================================================================
_sounddrv_init:
SOUNDDRV_INIT:
    DI
    PUSH AF
    PUSH BC
    PUSH DE
    PUSH HL
    PUSH IX
    PUSH IY

;	CALL GICINI		                ; GICINI	PSG initialization
    CALL $0090		                ; GICINI	PSG initialization

    ; Backup H.TIMI
;    LD HL,H_TIMI                    ; Transfer source
    LD HL,$FD9F                     ; Transfer source
    LD DE,SOUNDDRV_H_TIMI_BACKUP    ; Transfer destination
    LD BC,5                         ; Number of transfer bytes
    LDIR

    ; Rewrite H.TIMI
    LD A,$C3                        ; JP
    LD HL,SOUNDDRV_EXEC             ; Sound driver address
;    LD (H_TIMI+0),A
    LD ($FD9F+0),A
;    LD (H_TIMI+1),HL
    LD ($FD9F+1),HL

    ; Sound output settings
	LD A,7			                ; PSG register number=7 (channel settings)
	LD E,%10111111	                ; Channel ON/OFF settings 0:ON 1:OFF, 10+NOISE C~A+TONE C~A
                                    ; All initially OFF
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register

    ; Driver status initialization
    LD A,SOUNDDRV_STATE_STOP
    LD (SOUNDDRV_STATE),A

    ; Driver work area initialization
    LD HL,SOUNDDRV_WK_MIXING_TONE
    LD (HL),0

    LD HL,SOUNDDRV_WK_MIXING_NOISE
    LD (HL),0

    ; BGM/SFX work area initialization
    LD HL,SOUNDDRV_BGMWK
    LD B,SOUNDDRV_WORK_DATASIZE*6
SOUNDDRV_INIT_2:
    LD (HL),0
    INC HL
    DJNZ SOUNDDRV_INIT_2

    POP IY
    POP IX
    POP HL
    POP DE
    POP BC
    POP AF
    EI

    RET


; ====================================================================================================
; BGM PLAY
; IN  : HL = BGM data start address
;            BGM data structure is as follows:
;              Tempo: 1 byte
;              Track 1 data address: 2 bytes
;              Track 2 data address: 2 bytes
;              Track 3 data address: 2 bytes
; ====================================================================================================
_sounddrv_bgmplay:
SOUNDDRV_BGMPLAY:
    DI
    PUSH AF
    PUSH BC
    PUSH DE
    PUSH HL
    PUSH IX
    PUSH IY

    ; Initialize each channel
    PUSH HL
    XOR A
    CALL SOUNDDRV_GETWKADDR         ; HL <- Work area start address of target track
    PUSH HL                         ; IX <- HL
    POP IX
    POP HL

    ; Set BGM data to BGM track
    CALL SOUNDDRV_INITWK

    LD A,(SOUNDDRV_STATE)
    OR SOUNDDRV_STATE_PLAY          ; Set sound driver status to playing
    LD (SOUNDDRV_STATE),A

    POP IY
    POP IX
    POP HL
    POP DE
    POP BC
    POP AF
    EI

    RET


; ====================================================================================================
; SFX PLAY
; IN  : HL = SFX data start address
;            SFX data structure is as follows:
;              Tempo: 1 byte
;              Track 1 data address: 2 bytes (zero = none)
;              Track 2 data address: 2 bytes (zero = none)
;              Track 3 data address: 2 bytes (zero = none)
; ====================================================================================================
_sounddrv_sfxplay:
SOUNDDRV_SFXPLAY:
    DI
    PUSH AF
    PUSH BC
    PUSH DE
    PUSH HL
    PUSH IX
    PUSH IY

    ; Priority judgment
    PUSH HL                         ; HL -> Saved to SP
    XOR A                           ; BGM track start track number
    CALL SOUNDDRV_GETWKADDR         ; HL <- Work area start address of target track
    PUSH HL
    POP IX                          ; IX <- HL
    LD B,(IX+15)                    ; B <- Priority of BGM track being played

    LD A,4                          ; SFX track start track number
    CALL SOUNDDRV_GETWKADDR         ; HL <- Work area start address of target track
    PUSH HL
    POP IX                          ; IX <- HL
    LD A,(IX+15)                    ; A <- Priority of SFX track being played
    OR B                            ; A <- Priority value of BGM + SFX being played
    LD B,A                          ; B <- A (Measure to maintain consistency of CP behavior)
    POP HL                          ; HL <- Restore from SP (SFX data address)
    LD A,(HL)                       ; B <- SFX data priority
    CP B                            ; Priority value of BGM + SFX being played - SFX data priority
    JR C,SOUNDDRV_SFXPLAY_EXIT      ; If carry flag is ON, process ends without doing anything

    ; Set SFX data to SFX track
    CALL SOUNDDRV_INITWK

    LD A,(SOUNDDRV_STATE)
    OR SOUNDDRV_STATE_PLAY          ; Set sound driver status to playing
    LD (SOUNDDRV_STATE),A

SOUNDDRV_SFXPLAY_EXIT:
    POP IY
    POP IX
    POP HL
    POP DE
    POP BC
    POP AF
    EI

    RET

; ----------------------------------------------------------------------------------------------------
; Work area initialization process
; IN  : A = Priority value
;       HL = Address of BGM/SFX data to be set
;       IX = Address of BGM/SFX track work to be set
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_INITWK:
    LD B,3                          ; Number of channels

SOUNDDRV_INITWK_L1:
    LD A,(HL)                       ; A <- Priority
    INC HL
    LD E,(HL)                       ; DE <- Start address of BGM/SFX data
    INC HL
    LD D,(HL)

    ;   Wait counter
    ;   Initial value is 1 to ensure it becomes zero first
    LD (IX),1

    ;   Next BGM/SFX data read address
    ;   Set the start address as the initial value
    LD (IX+1),E
    LD (IX+2),D

    ;   BGM/SFX data start address
    LD (IX+3),E
    LD (IX+4),D

    ;   Detune value
    LD (IX+5),0

    ;   Mixing (bit0=Tone, bit1=Noise 0=On, 1=Off)
    LD (IX+6),%11

    ;   Priority
    LD (IX+15),A

    LD DE,16
    ADD IX,DE

    DJNZ SOUNDDRV_INITWK_L1

    RET


; ====================================================================================================
; PLAY STOP
; ====================================================================================================
_sounddrv_stop:
SOUNDDRV_STOP:
    DI
    PUSH AF
    PUSH BC
    PUSH DE
    PUSH HL
    PUSH IX
    PUSH IY

    LD A,(SOUNDDRV_STATE)
    AND SOUNDDRV_STATE_PAUSE        ; Set sound driver status to stop
                                    ; Keep pause state, so AND with 2
                                    ; - 0 AND 2 -> 0
                                    ; - 1 AND 2 -> 0
                                    ; - 2 AND 2 -> 2
                                    ; - 3 AND 2 -> 2
    LD (SOUNDDRV_STATE),A

    ; Set volume of all PSG channels to 0
    LD B,3
SOUNDDRV_STOP_L1:
    LD E,0                          ; E <- Data (volume)
    LD A,B                          ; A <- Track number
    ADD A,7                         ; Add 7 to specify PSG register 8~10
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register
    DJNZ SOUNDDRV_STOP_L1

    ; Clear all track work
    LD B,7                          ; BGM work area + SFX work area (including dummy)
SOUNDDRV_STOP_L2:
    LD A,B
    SUB 1                           ; Subtract 1 because track number starts from 0
    CALL SOUNDDRV_GETWKADDR
    PUSH HL
    POP IX
    LD (IX),$00                     ; Clear wait counter
    LD (IX+3),$00                   ; Clear track data start address
    LD (IX+4),$00
    LD (IX+15),$00                  ; Clear priority
    DJNZ SOUNDDRV_STOP_L2

    POP IY
    POP IX
    POP HL
    POP DE
    POP BC
    POP AF
    EI

    RET

; ====================================================================================================
; PLAY PAUSE
; ====================================================================================================
_sounddrv_pause:
SOUNDDRV_PAUSE:
    DI
    PUSH AF
;    PUSH BC
;    PUSH DE
;    PUSH HL
;    PUSH IX
;    PUSH IY

    LD A,(SOUNDDRV_STATE)
    CP SOUNDDRV_STATE_PAUSE
    JP NC,SOUNDDRV_RESUME_EXIT      ; If pause state, exit

    XOR SOUNDDRV_STATE_PAUSE        ; Set to pause state
    LD (SOUNDDRV_STATE),A

SOUNDDRV_PAUSE_EXIT:
;    POP IY
;    POP IX
;    POP HL
;    POP DE
;    POP BC
    POP AF
    EI

    RET

; ====================================================================================================
; PLAY RESUME
; ====================================================================================================
_sounddrv_resume:
SOUNDDRV_RESUME:
    DI
    PUSH AF
;    PUSH BC
;    PUSH DE
;    PUSH HL
;    PUSH IX
;    PUSH IY

    LD A,(SOUNDDRV_STATE)
    CP SOUNDDRV_STATE_PAUSE
    JP C,SOUNDDRV_RESUME_EXIT       ; If not pause state, exit

    XOR SOUNDDRV_STATE_PAUSE        ; Release pause state
    LD (SOUNDDRV_STATE),A

    LD B,3
SOUNDDRV_RESUME_L1:
    LD A,B                          ; A <- B (loop counter: 1~3)
    DEC A
    CALL SOUNDDRV_GETWKADDR         ; HL <- Work area address of corresponding BGM track
    PUSH HL                         ; IX <- HL (OK to destroy IX since it's last)
    POP IX

    CALL SOUNDDRV_SETPSG_NOISETONE  ; Noise tone (PSG register 6) setting
;    CALL SOUNDDRV_SETPSG_VOLUME     ; Volume (PSG register 8~10) setting
;    CALL SOUNDDRV_SETPSG_TONE       ; Tone (PSG register 0~5) setting
    DJNZ SOUNDDRV_RESUME_L1

SOUNDDRV_RESUME_EXIT:
;    POP IY
;    POP IX
;    POP HL
;    POP DE
;    POP BC
    POP AF
    EI

    RET


; ====================================================================================================
; DRIVER EXECUTE
; ====================================================================================================
_sounddrv_exec:
SOUNDDRV_EXEC:
    ; Sound driver status judgment
    LD A,(SOUNDDRV_STATE)           ; A <- Sound driver status
    OR A
    JP Z,SOUNDDRV_EXIT              ; If zero (stopped), exit
    CP SOUNDDRV_STATE_PAUSE
    JP NC,SOUNDDRV_ALLMUTE          ; Process during pause

    ; Process for each track
    XOR A
    CALL SOUNDDRV_CHEXEC
    LD A,1                          ; A <- 1 (BGM track 1 = ChB)
    CALL SOUNDDRV_CHEXEC
    LD A,2                          ; A <- 2 (BGM track 2 = ChC)
    CALL SOUNDDRV_CHEXEC
SOUNDDRV_EXEC_L1:
    LD A,4                          ; A <- 4 (SFX track 0 = ChA)
    CALL SOUNDDRV_CHEXEC
    LD A,5                          ; A <- 5 (SFX track 1 = ChB)
    CALL SOUNDDRV_CHEXEC
    LD A,6                          ; A <- 6 (SFX track 2 = ChC)
    CALL SOUNDDRV_CHEXEC

    ; Process for entire channel
    CALL SOUNDDRV_SETPSG_MIXING     ; Mixing (PSG register 7) setting process

SOUNDDRV_EXIT:
;    RET
    JP SOUNDDRV_H_TIMI_BACKUP

; ----------------------------------------------------------------------------------------------------
; Process during pause
; Set volume of channels 1~3 to zero
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_ALLMUTE:
    XOR A
    LD E,A                          ; E = Data to write (volume zero)
    LD A,8
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register
    LD A,9
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register
    LD A,10
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register
;    JP SOUNDDRV_EXIT
    JP SOUNDDRV_EXEC_L1

; ----------------------------------------------------------------------------------------------------
; Track data playback process
; IN  : A  = Track number (0~2, 4~6)
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_CHEXEC:
    LD D,A                          ; A -> D (Save track number in D register)

    CALL SOUNDDRV_GETWKADDR         ; HL <- Get start address of track work area
    PUSH HL                         ; IX <- HL
    POP IX
    
    ; Check track data start address
    LD A,(IX+3)
    OR (IX+4)
    RET Z                           ; If track data start address = zero (unregistered), exit   

    ; Subtract wait counter for sounding tone
    DEC (IX)
    RET NZ                          ; If result after -1 is not zero, tone is sounding, so exit

SOUNDDRV_CHEXEC_L2:
    ; Get music data for target channel
    ;   If sound is finished, get next data
    CALL SOUNDDRV_GETNEXTNATA       ; A <- Sequence data
    JP Z,SOUNDDRV_CHEXEC_L3         ; If zero flag is set, go to L3
                                    ; (If fetched data is end, zero flag is set)

SOUNDDRV_CHEXEC_L21:
    ; Branch by command
    CP 218                          ; Is data = 218 (detune value)?
    JP Z,SOUNDDRV_CHEXEC_CMD218     ; Go to detune value setting process

    CP 217                          ; Is data = 217 (mixing)?
    JP Z,SOUNDDRV_CHEXEC_CMD217     ; Go to mixing setting process

    CP 216                          ; Is data = 216 (noise tone)?
    JP Z,SOUNDDRV_CHEXEC_CMD216     ; Go to noise tone setting process

    CP 253                          ; Is data = 253 (loop start position)?
    JP Z,SOUNDDRV_CHEXEC_CMD253     ; Go to loop start position setting process

    CP 200                          ; Is data = 200~ (volume)?
    JP NC,SOUNDDRV_CHEXEC_CMD20X    ; Go to volume setting process

  
    ; Process when data = 0~190 (tone data)
    ;   Get corresponding data from tone table and set to PSG register 0~5
    ;   Get next data and set to wait counter
    LD B,0                          ; BC <- A (sequence data)
    LD C,A    
    LD HL,SOUNDDRV_TONETBL          ; HL <- Start address of tone table
    ADD HL,BC                       ; Tone data is 2 bytes, so index x 2
    ADD HL,BC

    LD A,(HL)                       ; A <- Tone data (lower byte)
    SUB (IX+5)                      ; Subtract detune value
    LD (IX+8),A                     ; Save tone data (lower byte) to work
    INC HL
    LD A,(HL)                       ; A <- Tone data (upper byte)
    LD (IX+9),A                     ; Save tone data (upper byte) to work


    LD A,D                          ; A <- D (track number)
    CP 3
    JR NC,SOUNDDRV_CHEXEC_L22       ; If 1 (= SFX), go to L22

    ;   Process for BGM track
    ;   Check SFX data start address set in SFX track work
    ;   If not $0000, SFX is playing, so only set wait counter without setting tone data
    ADD A,4                         ; Add 4 to track number to check SFX track
    CALL SOUNDDRV_GETWKADDR         ; Get start address of target track to HL

    INC HL                          ; @ToDo: Often need to find track data start address, want to review work structure (takes 21 states each time)
    INC HL
    INC HL

    LD A,(HL)                       ; Is target track start address = $0000?
    INC HL
    OR (HL)
    JR NZ,SOUNDDRV_CHEXEC_L23       ; If not zero, SFX is playing, don't set BGM tone, go to L24

SOUNDDRV_CHEXEC_L22:
    CALL SOUNDDRV_SETPSG_TONE       ; Tone (PSG register 0~5) setting

SOUNDDRV_CHEXEC_L23:
    ; Set wait counter for corresponding channel
    CALL SOUNDDRV_GETNEXTNATA       ; A <- Sequence data
    LD (IX),A                       ; Set wait counter to work

    RET

; ----------------------------------------------------------------------------------------------------
; Data end process
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_CHEXEC_L3:
    ; Determine if current track is BGM or SFX
    LD A,D                          ; A <- D (track number)
    CP 3
    JP NC,SOUNDDRV_CHEXEC_L4        ; If 1 (= SFX track), go to L4

    ; When BGM playback is finished
    LD (IX+3),$00                   ; Initialize track data start address in work area to zero
    LD (IX+4),$00
    LD (IX+15),$00                  ; Initialize priority to zero

    ; Check track data start address of all channels
    LD HL, SOUNDDRV_BGMWK
    LD BC, 3                        ; ch1wk
    ADD HL,BC
    LD A,(HL)
    INC HL
    ADD A,(HL)
    LD BC,15                        ; ch2wk
    ADD HL,BC
    ADD A,(HL)
    INC HL
    ADD A,(HL)
    LD BC,15                        ; ch3wk
    ADD HL,BC
    ADD A,(HL)
    INC HL
    ADD A,(HL)

    OR A
    JR NZ,SOUNDDRV_CHEXEC_L31

    LD A,SOUNDDRV_STATE_STOP        ; Update status to stop
    LD (SOUNDDRV_STATE),A

SOUNDDRV_CHEXEC_L31:
    RET

SOUNDDRV_CHEXEC_L4:
    ; When SFX playback is finished, restore corresponding BGM track state
    LD (IX+3),$00                   ; Initialize track data start address in work area to zero
    LD (IX+4),$00
    LD (IX+15),$00                  ; Initialize priority to zero

    LD A,D                          ; A <- D (track number)
    AND %00000011                   ; Convert track number to channel number (0~2)
    CALL SOUNDDRV_GETWKADDR         ; HL <- Work area address of corresponding BGM track
    PUSH HL                         ; IX <- HL (OK to destroy IX since it's last)
    POP IX

    LD A,(SOUNDDRV_STATE)
    AND SOUNDDRV_STATE_PAUSE
    RET NZ

    CALL SOUNDDRV_SETPSG_NOISETONE  ; Noise tone (PSG register 6) setting
    CALL SOUNDDRV_SETPSG_VOLUME     ; Volume (PSG register 8~10) setting
    CALL SOUNDDRV_SETPSG_TONE       ; Tone (PSG register 0~5) setting

;SOUNDDRV_CHEXEC_EXIT:
    RET

; ----------------------------------------------------------------------------------------------------
; Command: 200~215 (Volume) setting
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_CHEXEC_CMD20X:
    ; Volume setting process
    ;   Since command data - 200 becomes volume value, calculate and set to work area
    ;   Then process next sequence data
    SUB 200                         ; A = A - 200 (0~15 volume value)
    LD (IX+7),A                     ; Save volume value to work

    LD A,D                          ; A <- D (track number)

    ; Determine if current track is BGM or SFX
    CP 3
    JP NC,SOUNDDRV_CHEXEC_CMD20X_L1 ; If 1 (= SFX track), go to L1

    ADD A,4                         ; Add A = A + 4 to track number to check SFX track
    CALL SOUNDDRV_GETWKADDR         ; Get start address of target track to HL

    INC HL                          ; @ToDo: Often need to find track data start address, want to review work structure (takes 21 states each time)
    INC HL
    INC HL

    LD A,(HL)
    INC HL
    OR (HL)                         ; Is target track start address = $0000?
    JR NZ,SOUNDDRV_CHEXEC_CMD20X_L2 ; If not zero, SFX is playing, go to CMD20X_1

SOUNDDRV_CHEXEC_CMD20X_L1:
    CALL SOUNDDRV_SETPSG_VOLUME     ; Set PSG register 8~10

SOUNDDRV_CHEXEC_CMD20X_L2:
    JP SOUNDDRV_CHEXEC_L2

; ----------------------------------------------------------------------------------------------------
; Command: 217 (Mixing value) setting
;   Get next sequence data and set to work area, also set to PSG register 7
;   Then process next sequence data
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_CHEXEC_CMD217:
    CALL SOUNDDRV_GETNEXTNATA       ; A <- Sequence data (mixing value)
    LD (IX+6),A

    JP SOUNDDRV_CHEXEC_L2

; ----------------------------------------------------------------------------------------------------
; Command: 216 (Noise tone value) setting
;   Get next sequence data and set to work area
;   Then process next sequence data
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_CHEXEC_CMD216:
    CALL SOUNDDRV_GETNEXTNATA       ; A <- Sequence data (noise tone value)
    LD (IX+10),A                    ; Save noise tone value to work
    CALL SOUNDDRV_SETPSG_NOISETONE  ; Noise tone (PSG register 6) setting

    JP SOUNDDRV_CHEXEC_L2

; ----------------------------------------------------------------------------------------------------
; Command: 218 (Detune value) setting
;   Get next sequence data and set to work area
;   Then process next sequence data
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_CHEXEC_CMD218:
    CALL SOUNDDRV_GETNEXTNATA       ; A <- Sequence data (detune value)
    LD (IX+5),A

    JP SOUNDDRV_CHEXEC_L2

; ----------------------------------------------------------------------------------------------------
; Command: 253 (Loop start position) setting
;   Set current address + 1 to work
;   Then process next sequence data
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_CHEXEC_CMD253:
    LD C,(IX+1)                     ; BC <- Next sequence data address set at this point
    LD B,(IX+2)
    
    LD (IX+3),C                     ; Rewrite track data start address
    LD (IX+4),B

    JP SOUNDDRV_CHEXEC_L2

; ----------------------------------------------------------------------------------------------------
; Tone (PSG register 0~5) setting
;   Channel A: PSG register 0,1
;   Channel B: PSG register 2,3
;   Channel C: PSG register 4,5
;   Find PSG register 0~5 setting value from current work setting and execute WRTPSG
; IN  : D = Track number
;       IX = Work area start address of target track
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_SETPSG_TONE:
    LD A,D                          ; A <- Value saved in D register (track number)
    AND %00000011                   ; Lower 2 bits as channel number
    ADD A,A                         ; PSG register number = 0/2/4 (lower 8 bits)
    LD E,(IX+8)                     ; E <- Work tone (lower byte)
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register

    INC A                           ; PSG register number = 1/3/5 (upper 4 bits)
    LD E,(IX+9)                     ; E <- Work tone (upper byte)
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register

    RET

; ----------------------------------------------------------------------------------------------------
; Noise tone (PSG register 6) setting process
;   Common to all channels
;   Find PSG register 6 setting value from current work setting and execute WRTPSG
; IN  : IX = Work area start address of target track
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_SETPSG_NOISETONE:
    LD E,(IX+10)
    LD A,6
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register

    RET

; ----------------------------------------------------------------------------------------------------
; Volume (PSG register 8~10) setting process
;   Channel A: PSG register 8
;   Channel B: PSG register 9
;   Channel C: PSG register 10
;   Find PSG register 8~10 setting value from current work setting and execute WRTPSG
; IN  : D = Track number
;       IX = Work area start address of target track
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_SETPSG_VOLUME:
    LD A,D                          ; A <- Track number (0~2, 4~6)
    AND %00000011                   ; Lower 2 bits as channel number
    ADD A,8                         ; Add 8 to specify PSG register 8~10
    LD E,(IX+7)
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register

    RET

; ----------------------------------------------------------------------------------------------------
; Mixing (PSG register 7) setting process
;   Common to all channels
;   Find PSG register 7 setting value from current work setting and execute WRTPSG
;   Register 7 setting value is as follows (0=On, 1=Off)
;     xx000000
;       |||||bit0: ChA Tone
;       ||||bit1: ChB Tone
;       |||bit2: ChC Tone
;       ||bit3: ChA Noise
;       |bit4: ChB Noise
;       bit5: ChC Noise
;   Set in work of each track as follows
;     00
;     |bit0: Tone
;     bit1: Noise
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_SETPSG_MIXING:
    LD B,3                          ; Loop count

    XOR A
    LD (SOUNDDRV_WK_MIXING_TONE),A  ; A -> PSG register 7 WK (bit0~2: Tone setting) initialize
    LD (SOUNDDRV_WK_MIXING_NOISE),A ; A -> PSG register 7 WK (bit3~5: Noise setting) initialize

SOUNDDRV_SETPSG_MIXING_L1:
    ; Set mixing value address for each track
    ;   Process Ch2, 1, 0 in order
    ;   Find track data start address of SFX track
    LD A,B                          ; A <- B (1~3)
    ADD A,3                         ; Add 3 to A, make it track number 4~6 (SFX track 1~3)
    CALL SOUNDDRV_GETWKADDR         ; HL <- Start address of SFX work area
    INC HL                          ; @ToDo: Often need to find track data start address, want to review work structure (takes 21 states each time)
    INC HL
    INC HL

    ;   Determine SFX track data start address
    LD A,(HL)                       ; Is track data start address = $0000?
    INC HL
    OR (HL)
    JR NZ,SOUNDDRV_SETPSG_MIXING_L2     ; If not zero, SFX track is set, go to next process

    ;   Find track data start address of BGM track
    LD A,B                          ; A <- B (1~3)
    SUB 1                           ; Subtract 1 from A, make it track number 0~2
    CALL SOUNDDRV_GETWKADDR         ; HL <- Start address of BGM work area
    INC HL                          ; @ToDo: Often need to find track data start address, want to review work structure (takes 21 states each time)
    INC HL
    INC HL

    ;   Determine BGM track data start address
    LD A,(HL)                       ; Is track data start address = $0000?
    INC HL
    OR (HL)
    JR NZ,SOUNDDRV_SETPSG_MIXING_L2     ; If not zero, BGM track is set, skip next process

    ; If BGM and SFX are not set, set mixing value to %11 (Noise, Tone=Off)
    LD D,%11
    JR SOUNDDRV_SETPSG_MIXING_L3

SOUNDDRV_SETPSG_MIXING_L2:
    ; Get mixing value of each track and set to work
    INC HL
    INC HL
    LD D,(HL)                       ; D <- Mixing value of target track

SOUNDDRV_SETPSG_MIXING_L3:
    ;   Tone mixing value
    SRL D                           ; Right shift D register 1 bit, bit0 of original value -> carry flag
    LD A,(SOUNDDRV_WK_MIXING_TONE)  ; A <- PSG register 7 WK (bit0~2: Tone setting)
    RLA                             ; Left rotate A register 1 bit, carry flag -> bit0
    LD (SOUNDDRV_WK_MIXING_TONE),A  ; A -> PSG register 7 WK (bit0~2: Tone setting)

    ;   Noise mixing value
    SRL D                           ; Right shift D register 1 bit, bit1 of original value -> carry flag
    LD A,(SOUNDDRV_WK_MIXING_NOISE) ; A <- PSG register 7 WK (bit3~5: Noise setting)
    RLA                             ; Left rotate A register 1 bit, carry flag -> bit0
    LD (SOUNDDRV_WK_MIXING_NOISE),A ; A -> PSG register 7 WK (bit3~5: Noise setting)

    DJNZ SOUNDDRV_SETPSG_MIXING_L1

    ; Find value to set to register 7
    LD A,(SOUNDDRV_WK_MIXING_TONE)  ; A <- PSG register 7 WK (bit0~2: Tone setting)
    LD E,A                          ; E <- A

    LD A,(SOUNDDRV_WK_MIXING_NOISE) ; A <- PSG register 7 WK (bit0~2: Tone setting)
    SLA A                           ; Left shift 3 bits -> move bit2~0 data to bit5~3
    SLA A
    SLA A
    OR E                            ; Add tone value
    OR %10000000                    ; Set bit7~6
    LD E,A
    LD A,7
;	CALL WRTPSG		                ; BIOS WRTPSG  Write data to PSG register
    CALL $0093                      ; BIOS WRTPSG  Write data to PSG register

    RET

; ----------------------------------------------------------------------------------------------------
; Get data from next track data read address
; At the same time, update track data read address
; If data is end (=$FF), restore track data read address to start address
; IN  : IX = Track work area start address
; OUT : A = Track data
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_GETNEXTNATA:
    ; Get track data
    LD C,(IX+1)                     ; BC <- Track data read address
    LD B,(IX+2)
    LD A,(BC)                       ; A <- Music data

    ; End determination
    CP $FF                          ; Is data = $FF?
    RET Z                           ; If end, process ends as is

    ; Loop determination
    CP $FE                          ; Is data = $FE?
    JR NZ,SOUNDDRV_GETNEXTNATA_L2   ; If not $FE, go to L2

    ; Restore track data to loop start
    INC A                           ; Clear zero flag
                                    ; (Unconditionally add 1 to A register, A is guaranteed to be less than $FF before coming here, so zero flag is always OFF)
    LD C,(IX+3)                     ; BC <- Track data start address
    LD B,(IX+4)
    LD A,(BC)                       ; Re-read track data to A register

SOUNDDRV_GETNEXTNATA_L2:
    ; Increment next track data read address and save
    INC BC
    LD (IX+1),C                     ; BC -> Next track data read address
    LD (IX+2),B

    RET

; ----------------------------------------------------------------------------------------------------
; Find BGM/SFX work area address
; IN  : A = Track number (0~2, 4~6)
; OUT : HL = Work area start address of target track
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_GETWKADDR:
    PUSH BC
    LD HL,SOUNDDRV_BGMWK            ; HL <- Start address of BGM work area

    OR A                            ; Is it zero?
    JR Z,SOUNDDRV_GETWKADDR_L1      ; If zero, no calculation needed, go to L2

    SLA A                           ; A = A * 16 (work area size)
    SLA A
    SLA A
    SLA A

SOUNDDRV_GETWKADDR_L1:
    LD B,0
    LD C,A
    ADD HL,BC                       ; HL <- Address of work area of target track

    POP BC
    RET


; ====================================================================================================
; Constant area
; Stored in ROM
; ====================================================================================================
SECTION rodata_user

; BIOS address definitions
;INCLUDE "include/msxbios.inc"

SOUNDDRV_STATE_STOP:    EQU 0       ; Sound driver status: Stop
SOUNDDRV_STATE_PLAY:    EQU 1       ; Sound driver status: Playing
SOUNDDRV_STATE_PAUSE:   EQU 2       ; Sound driver status: Pause

SOUNDDRV_WORK_DATASIZE: EQU 16      ; Sound driver 1ch work area size


; ----------------------------------------------------------------------------------------------------
; Tone table
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_TONETBL:
;          C   C+    D   D+    E    F   F+    G   G+    A   A+    B
	dw  3420,3229,3047,2876,2715,2562,2419,2283,2155,2034,1920,1812 ;o1  0~ 11
	dw  1710,1614,1524,1438,1357,1281,1209,1141,1077,1017, 960, 906 ;o2 12~ 23
	dw   855, 807, 762, 719, 679, 641, 605, 571, 539, 508, 480, 453 ;o3 24~ 35
	dw   428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226 ;o4 36~ 47
	dw   214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113 ;o5 48~ 59
	dw   107, 101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57 ;o6 60~ 71
	dw    53,  50,  48,  45,  42,  40,  38,  36,  34,  32,  30,  28 ;o7 72~ 83
	dw    27,  25,  24,  22,  21,  20,  19,  18,  17,  16,  15,  14 ;o8 84~ 95


; ====================================================================================================
; Work area
; Set to zero in RAM by crt at program startup
; ====================================================================================================
SECTION bss_user

SOUNDDRV_WORKAREA:
; ----------------------------------------------------------------------------------------------------
; H.TIMI hook backup
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_H_TIMI_BACKUP:
    DEFS 5    

; ----------------------------------------------------------------------------------------------------
; Driver status
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_STATE:
_sounddrv_state:
    DB  SOUNDDRV_STATE_STOP         ; Sound driver status initial value

; ----------------------------------------------------------------------------------------------------
; Driver work area
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_WK_MIXING_TONE:
    DB  %00000000                   ; PSG register 7 WK (bit7~5: Tone setting) calculation
SOUNDDRV_WK_MIXING_NOISE:
    DB  %00000000                   ; PSG register 7 WK (bit7~5: Noise setting) calculation

; ----------------------------------------------------------------------------------------------------
; BGM work area
; ----------------------------------------------------------------------------------------------------
SOUNDDRV_BGMWK:
    ; BGM track 1 (= ChA)
    DB  $00                         ; +0 Wait counter (1 tone = n/60 sec)
    DW  $0000                       ; +1,2 Track data read address
    DW  $0000                       ; +3,4 Track data start address
    DB  $00                         ; +5 Detune
    DB  $00                         ; +6 Mixing (bit0=Tone, bit1=Noise 0=On, 1=Off)
    DB  $00                         ; +7 Volume
    DB  $00                         ; +8 Tone (lower byte)
    DB  $00                         ; +9 Tone (upper byte)
    DB  $00                         ; +A Noise tone
    DB  $00                         ; +B Reserved
    DB  $00                         ; +C Reserved
    DB  $00                         ; +D Reserved
    DB  $00                         ; +E Reserved
    DB  $00                         ; +F Reserved
    ; BGM track 2 (= ChB)
    DB  $00                         ; +0 Wait counter (1 tone = n/60 sec)
    DW  $0000                       ; +1,2 Track data read address
    DW  $0000                       ; +3,4 Track data start address
    DB  $00                         ; +5 Detune
    DB  $00                         ; +6 Mixing (bit0=Tone, bit1=Noise 0=On, 1=Off)
    DB  $00                         ; +7 Volume
    DB  $00                         ; +8 Tone (lower byte)
    DB  $00                         ; +9 Tone (upper byte)
    DB  $00                         ; +A Noise tone
    DB  $00                         ; +B Reserved
    DB  $00                         ; +C Reserved
    DB  $00                         ; +D Reserved
    DB  $00                         ; +E Reserved
    DB  $00                         ; +F Reserved
    ; BGM track 3 (Ch C)
    DB  $00                         ; +0 Wait counter (1 tone = n/60 sec)
    DW  $0000                       ; +1,2 Track data read address
    DW  $0000                       ; +3,4 Track data start address
    DB  $00                         ; +5 Detune
    DB  $00                         ; +6 Mixing (bit0=Tone, bit1=Noise 0=On, 1=Off)
    DB  $00                         ; +7 Volume
    DB  $00                         ; +8 Tone (lower byte)
    DB  $00                         ; +9 Tone (upper byte)
    DB  $00                         ; +A Noise tone
    DB  $00                         ; +B Reserved
    DB  $00                         ; +C Reserved
    DB  $00                         ; +D Reserved
    DB  $00                         ; +E Reserved
    DB  $00                         ; +F Reserved
