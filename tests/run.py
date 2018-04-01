# coding: utf-8

import sys
import subprocess

# la `key` son los datos de STDIN
# cada lista tiene los parámetros cmd line y el resultado esperado del contador
TEST_CASES = {
    'hola': [
        ['-b', 4],
        ['-c', 4],
        ['-w', 1],
        ['-l', 0],
    ],
    'hola mundo!': [
        ['-b', 11],
        ['-c', 11],
        ['-w', 2],
        ['-l', 0],
    ],
    'con UTF-8: áéíóú': [
        ['-b', 21],
        ['-c', 16],
        ['-w', 3],
        ['-l', 0],
    ],
    'con\nmas\nlineas': [
        ['-b', 14],
        ['-c', 14],
        ['-w', 3],
        ['-l', 2],
    ]
}

CMD = "printf '{data}' | ./{binary} {params}"

if __name__ == '__main__':
    binary = sys.argv[1]

    print('Ejecutando pruebas...')

    for input_data, test_cases in TEST_CASES.items():
        for params, expected in test_cases:
            cmd = CMD.format(data=input_data, binary=binary, params=params)
            print('test: \033[94m%r\033[0m' % cmd)

            try:
                output = subprocess.check_output(cmd, shell=True)
            except subprocess.CalledProcessError:
                print('  Error al ejecutar el binario')
                exit(1)

            output = output.decode('utf-8')

            assert output[-1] == '\n', output
            obtained = int(output[:-1])
            if obtained != expected:
                print('\033[1;91m  Obtained: {}\n  Expected: {}\033[0m'.format(obtained, expected))
                exit(1)
            else:
                print('\033[1;92m  OK\033[0m')
