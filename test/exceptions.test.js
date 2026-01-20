'use strict';

const assert = require('node:assert').strict;
const { describe, it } = require('node:test');
const util = require('node:util');
const exec = util.promisify(require('node:child_process').exec);


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
		const exceptionName = process.platform === 'win32' ? 'ACCESS_VIOLATION' : 'SIGSEGV';
		assert.ok(response.includes(exceptionName));
	});
	
	// On Unix, the stacktrace is empty sometimes
	if (process.platform === 'win32') {
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
	if (process.platform === 'win32' || process.platform === 'linux') {
		it('reports divisions by zero (int)', async () => {
			let response = await runAndGetError('causeDivisionInt');
			const exceptionName = process.platform === 'win32' ? 'INT_DIVIDE_BY_ZERO' : 'SIGFPE';
			assert.ok(response.includes(exceptionName));
		});
	}
	
	// On Unix, this hangs for some reason
	if (process.platform === 'win32') {
		it('reports stack overflows', async () => {
			let response = await runAndGetError('causeOverflow');
			const exceptionName = process.platform === 'win32' ? 'STACK_OVERFLOW' : 'SIGSEGV';
			assert.ok(response.includes(exceptionName));
		});
	}
	
	it('reports illegal operations', async () => {
		let response = await runAndGetError('causeIllegal');
		const exceptionName = process.platform === 'win32' ? 'ILLEGAL_INSTRUCTION' : 'SIGILL';
		assert.ok(response.includes(exceptionName));
	});
});
