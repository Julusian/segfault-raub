'use strict';

const assert = require('node:assert').strict;
const { describe, it } = require('node:test');
const util = require('node:util');
const exec = util.promisify(require('node:child_process').exec);

// The exception *name* depends on the OS only (the addon gates on `_WIN32`,
// which covers all Windows including ARM64). Some tests are additionally
// flaky/unsupported on ARM, so they are gated on `!isArm`.
const isWindows = process.platform === 'win32';
const isLinux = process.platform === 'linux';
const isArm = process.arch === 'arm64' || process.arch === 'arm';


const runAndGetError = async (name) => {
	let response = '';
	try {
		const { stderr, stdout } = await exec(`node -e "require('.').${name}()"`);
		response = stderr + stdout;
	} catch (error) {
		response = error.message;
	}
	return response;
};

describe('Exceptions', () => {
	it('reports segfaults', async () => {
		let response = await runAndGetError('causeSegfault');
		const exceptionName = isWindows ? 'ACCESS_VIOLATION' : 'SIGSEGV';
		assert.ok(response.includes(exceptionName));
	});
	
	// On Unix, the stacktrace is empty sometimes
	if (isWindows && !isArm) {
		it('shows symbol names in stacktrace', async () => {
			let response = await runAndGetError('causeSegfault');
			assert.match(response, /segfault::causeSegfault/);
		});
		
		it('shows module names in stacktrace', async () => {
			let response = await runAndGetError('causeSegfault');
			assert.ok(response.includes('[node.exe]'));
			assert.ok(response.includes('[julusian_segfault_handler.node]'));
		});
	}
	
	// On ARM this fails
	if ((isWindows || isLinux) && !isArm) {
		it('reports divisions by zero (int)', async () => {
			let response = await runAndGetError('causeDivisionInt');
			const exceptionName = isWindows ? 'INT_DIVIDE_BY_ZERO' : 'SIGFPE';
			assert.ok(response.includes(exceptionName));
		});
	}
	
	// On Unix, this hangs for some reason
	if (isWindows && !isArm) {
		it('reports stack overflows', async () => {
			let response = await runAndGetError('causeOverflow');
			const exceptionName = isWindows ? 'STACK_OVERFLOW' : 'SIGSEGV';
			assert.ok(response.includes(exceptionName));
		});
	}
	
	it('reports illegal operations', async () => {
		let response = await runAndGetError('causeIllegal');
		const exceptionName = isWindows ? 'ILLEGAL_INSTRUCTION' : 'SIGILL';
		assert.ok(response.includes(exceptionName));
	});
});
