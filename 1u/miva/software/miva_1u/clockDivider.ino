void fireGates(byte gate_number, int c_tick, int c_div) {

switch (c_tick) {
  case 0:
     writeGate(gate_number, HIGH);
    break;
  case 3:
     writeGate(gate_number, LOW);
    break;
  case 6:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 9:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  case 12:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 15:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, LOW);
    }
    break;
  case 16:
     if(c_div == 16) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 18:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 19:
    if(c_div == 16) {
      writeGate(gate_number, LOW);
    }
    break;
  case 21:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  case 24:
    if(c_div == 6 || c_div == 12 || c_div == 24) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 27:
    if(c_div == 6 || c_div == 12 || c_div == 24) {
      writeGate(gate_number, LOW);
    }
    break;
  case 30:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 32:
    if(c_div == 16) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 33:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  case 35:
    if(c_div == 16) {
      writeGate(gate_number, LOW);
    }
    break;
  case 36:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 39:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, LOW);
    }
    break;
  case 42:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 45:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  case 48:
    if(c_div == 6 || c_div == 12 || c_div == 16 || c_div == 24 || c_div == 48) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 51:
    if(c_div == 6 || c_div == 12 || c_div == 16 || c_div == 24 || c_div == 48) {
      writeGate(gate_number, LOW);
    }
    break;
  case 54:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 57:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  case 60:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 63:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, LOW);
    }
    break;
  case 64:
    if(c_div == 16) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 66:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 67:
    if(c_div == 16) {
      writeGate(gate_number, LOW);
    }
    break;
  case 69:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  case 72:
    if(c_div == 6 || c_div == 12 || c_div == 24) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 75:
    if(c_div == 6 || c_div == 12 || c_div == 24) {
      writeGate(gate_number, LOW);
    }
    break;
  case 78:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 80:
    if(c_div == 16) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 81:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  case 83:
    if(c_div == 16) {
      writeGate(gate_number, LOW);
    }
    break;
  case 84:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 87:
    if(c_div == 6 || c_div == 12) {
      writeGate(gate_number, LOW);
    }
    break;
  case 90:
    if(c_div == 6) {
      writeGate(gate_number, HIGH);
    }
    break;
  case 93:
    if(c_div == 6) {
      writeGate(gate_number, LOW);
    }
    break;
  default:
    break; 
}

}
