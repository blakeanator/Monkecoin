import hashlib, binascii, struct, array, os, time, sys, optparse, codecs, multiprocessing
from construct import *

MAX_INT32_VALUE = pow(2, 32) - 1

def main():

  options = get_args()

  input_script = create_input_script(options.timestamp)
  print("input script: " +  codecs.encode(input_script, 'hex').decode('ascii'))

  output_script = create_output_script(options.pubkey)
  tx = create_transaction(input_script, output_script, options)

  # hash merkle root is the sha3 hash of the transaction(s) 
  hash_merkle_root = hashlib.sha3_256(tx).digest()

  print_block_info(options, hash_merkle_root)
  block_header = create_block_header(hash_merkle_root, options.time, options.bits, options.nonce)

  # https://en.bitcoin.it/wiki/Difficulty
  target = (options.bits & 0xffffff) * 2 ** (8 * ((options.bits >> 24) - 3))
  #target = 0x00ffff * 2**(8*(0x1d - 3))
  print("Target: 0x" + hex(target)[2:].zfill(64))

  cpu_count = multiprocessing.cpu_count()
  max_nonce = MAX_INT32_VALUE
  nonce_cpu_iterations = (max_nonce - options.nonce) / cpu_count

  hash_rates = multiprocessing.Array('I', cpu_count)
  nonce = multiprocessing.Value('I', 0)
  processes = []

  for i in range(cpu_count):

    cpu_start_nonce = int(i * nonce_cpu_iterations) + options.nonce
    cpu_end_nonce = int(((i + 1) * nonce_cpu_iterations) - 1) + options.nonce

    process = multiprocessing.Process(target=generate_hash, args=(block_header, cpu_start_nonce, cpu_end_nonce, options.bits, target, hash_rates, i, nonce))
    processes.append(process)
    process.start()

  start_time = time.time()
  last_updated_time = start_time

  while nonce.value == 0:
    now = time.time()
    if now >= last_updated_time + 1:
      last_updated_time = now
      combined_hash_rate = 0
      for u in hash_rates:
        combined_hash_rate += u
      if combined_hash_rate > 0:
        full_cycle_time = (MAX_INT32_VALUE - options.nonce) / combined_hash_rate
        print(str(cpu_count) + " Threads, rate: " + convert_hashrate_to_text(combined_hash_rate) + ", elapsed time: " + convert_seconds_to_text(last_updated_time - start_time) + ", est full cycle: " + convert_seconds_to_text(full_cycle_time), end="\r", flush=True)

  elapsed_time = time.time() - start_time

  data_block = block_header[0:len(block_header) - 4] + struct.pack('<I', nonce.value)
  header_hash = hashlib.sha3_256(data_block).digest()[::-1]

  if last_updated_time > start_time:
    print ("\033[K\033[F")

  print("--------------------------------------------------------------------------")
  print("Hash:   0x" + codecs.encode(header_hash, 'hex').decode('ascii'))
  print("nonce:  " + str(nonce.value))
  print("--------------------------------------------------------------------------")
  print("Genesis hash found in " + convert_seconds_to_text(elapsed_time))

  for proc in processes:
    proc.terminate()


def get_args():
  parser = optparse.OptionParser()
  parser.add_option("-t", "--time", dest="time", default=int(time.time()), 
                   type="int", help="the (unix) time when the genesisblock is created")
  parser.add_option("-z", "--timestamp", dest="timestamp", default="CNN 04/09/2021 Elon Musk's Neuralink claims monkeys can play Pong using just their minds",
                   type="string", help="the pszTimestamp found in the coinbase of the genesisblock")
  parser.add_option("-n", "--nonce", dest="nonce", default=0,
                   type="int", help="the first value of the nonce that will be incremented when searching the genesis hash")
  parser.add_option("-p", "--pubkey", dest="pubkey", default="4dd2d14bc8799aebc9dfc0f1e5d1c16df8f16beadd77e4d18662c2f1e3d1c34dd3f14bca79e2a97175efaee9dfc4d1e1f1c56df6d16bcbdde8e08662c0f1e5d1c1",
                   type="string", help="the pubkey found in the output script")
  parser.add_option("-v", "--value", dest="value", default=0,
                   type="int", help="the value in coins for the output, full value (monkecoin: 3200000000)")
  parser.add_option("-b", "--bits", dest="bits", default=0x1d00ffff,
                   type="int", help="the target in compact representation, associated to a difficulty of 1")

  (options, args) = parser.parse_args()
  return options


def create_input_script(psz_timestamp):
  psz_prefix = ""
  #use OP_PUSHDATA1 if required
  if len(psz_timestamp) > 76: psz_prefix = '4c'

  script_prefix = '04ffff001d0104' + psz_prefix + codecs.encode(bytes(chr(len(psz_timestamp)), encoding='utf8'), 'hex').decode('ascii')
  return codecs.decode(script_prefix + codecs.encode(bytes(psz_timestamp, encoding='utf8'), 'hex').decode('ascii'), 'hex')


def create_output_script(pubkey):
  script_len = '41'
  OP_CHECKSIG = 'ac'
  return codecs.decode((script_len + pubkey + OP_CHECKSIG), 'hex')


def create_transaction(input_script, output_script, options):
  transaction = Struct(
    "version" / Bytes(2),
    "num_inputs" / Byte,
    "prev_output" / Bytes(32),
    "prev_out_idx" / Int32ub,
    "input_script_len" / Byte,
    "input_script" / Bytes(len(input_script)),
    "sequence" / Int32ub,
    "num_outputs" / Byte,
    "out_value" / Bytes(8),
    "output_script_len" / Byte,
    "output_script" / Bytes(0x43),
    "locktime" / Int32ub,
    "donationwalletindex" / Int16ub
  )

  tx = transaction.parse(b'\x00' * (127 + len(input_script)))
  tx.version             = struct.pack('<H', 1)
  tx.num_inputs          = 1
  tx.prev_output         = struct.pack('<qqqq', 0,0,0,0)
  tx.prev_out_idx        = 0xFFFFFFFF
  tx.input_script_len    = len(input_script)
  tx.input_script        = input_script
  tx.sequence            = 0xFFFFFFFF
  tx.num_outputs         = 1
  tx.out_value           = struct.pack('<q' , options.value)
  tx.output_script_len   = 0x43
  tx.output_script       = output_script
  tx.locktime            = 0
  tx.donationwalletindex = 0 
  return transaction.build(tx)


def create_block_header(hash_merkle_root, time, bits, nonce):
  block_header = Struct(
    "version" / Bytes(2),
    "hash_prev_block" / Bytes(32),
    "hash_merkle_root" / Bytes(32),
    "time" / Bytes(4),
    "bits" / Bytes(4),
    "nonce" / Bytes(4)
  )

  genesisblock = block_header.parse(b'\x00' * 78)
  genesisblock.version          = struct.pack('<H', 1)
  genesisblock.hash_prev_block  = struct.pack('<qqqq', 0,0,0,0)
  genesisblock.hash_merkle_root = hash_merkle_root
  genesisblock.time             = struct.pack('<I', time)
  genesisblock.bits             = struct.pack('<I', bits)
  genesisblock.nonce            = struct.pack('<I', nonce)
  return block_header.build(genesisblock)


# https://en.bitcoin.it/wiki/Block_hashing_algorithm                    
def generate_hash(data_block, start_nonce, end_nonce, bits, target, global_hash_rates, i, nonce):
  try_nonce       = start_nonce
  last_updated    = time.time()
  last_nonce      = try_nonce
  last_hashrate   = 0

  while try_nonce <= end_nonce:
    header_hash = hashlib.sha3_256(data_block).digest()[::-1]
    now = time.time()
    if now >= last_updated + 1:
      last_hashrate = try_nonce - last_nonce
      last_nonce = try_nonce
      global_hash_rates[i] = last_hashrate
      last_updated = now
    if int(codecs.encode(header_hash, 'hex'), 16) < target:
      nonce.value = try_nonce
      break
    elif try_nonce <= MAX_INT32_VALUE:
      try_nonce += 1
      data_block = data_block[0:len(data_block) - 4] + struct.pack('<I', try_nonce)
    else:
      break
  return


def convert_hashrate_to_text(hashrate):
  if hashrate > 1000000000:
    return str(round(hashrate * 0.000000001, 1)) + " Gh/s"
  elif hashrate > 1000000:
    return  str(round(hashrate * 0.000001, 1)) + " Mh/s"
  elif hashrate > 1000:
    return str(round(hashrate * 0.001, 1)) + " Kh/s"
  else:
    return str(hashrate) + " h/s"


def convert_seconds_to_text(generation_time):
  if generation_time > 3600:
    return str(round(generation_time / 3600, 1)) + " hours"
  elif generation_time > 60:
    return str(round(generation_time / 60, 1)) + " minutes"
  else:
    return str(round(generation_time, 1)) + " seconds"


def print_block_info(options, hash_merkle_root):
  print("merkle hash: 0x"  + codecs.encode(hash_merkle_root[::-1], 'hex').decode('ascii'))
  print("pszTimestamp: "   + options.timestamp)
  print("pubkey: "         + options.pubkey)
  print("time:   "         + str(options.time))
  print("bits:   "         + str(hex(options.bits)))


main()